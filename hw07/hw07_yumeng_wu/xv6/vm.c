#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "elf.h"

extern char data[];  // defined by kernel.ld
pde_t *kpgdir;  // for use in scheduler()

uint shm_tab[MAX_SHM_NUMBER_TOTAL] = {0};
uint shm_cnt[MAX_SHM_NUMBER_TOTAL] = {0};
int next_shm_tad_id = 0;  // global shm_tab id

// Set up CPU's kernel segment descriptors.
// Run once on entry on each CPU.
void
seginit(void)
{
  struct cpu *c;

  // Map "logical" addresses to virtual addresses using identity map.
  // Cannot share a CODE descriptor for both kernel and user
  // because it would have to have DPL_USR, but the CPU forbids
  // an interrupt from CPL=0 to DPL=3.
  c = &cpus[cpuid()];
  c->gdt[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0);
  c->gdt[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0);
  c->gdt[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, DPL_USER);
  c->gdt[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, DPL_USER);
  lgdt(c->gdt, sizeof(c->gdt));
}

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)P2V(PTE_ADDR(*pde));
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table
    // entries, if necessary.
    *pde = V2P(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int
mappages(pde_t *pgdir, void *va, uint size, uint pa, int perm)
{
  char *a, *last;
  pte_t *pte;

  a = (char*)PGROUNDDOWN((uint)va);
  last = (char*)PGROUNDDOWN(((uint)va) + size - 1);
  for(;;){
    if((pte = walkpgdir(pgdir, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}

// There is one page table per process, plus one that's used when
// a CPU is not running any process (kpgdir). The kernel uses the
// current process's page table during system calls and interrupts;
// page protection bits prevent user code from using the kernel's
// mappings.
//
// setupkvm() and exec() set up every page table like this:
//
//   0..KERNBASE: user memory (text+data+stack+heap), mapped to
//                phys memory allocated by the kernel
//   KERNBASE..KERNBASE+EXTMEM: mapped to 0..EXTMEM (for I/O space)
//   KERNBASE+EXTMEM..data: mapped to EXTMEM..V2P(data)
//                for the kernel's instructions and r/o data
//   data..KERNBASE+PHYSTOP: mapped to V2P(data)..PHYSTOP,
//                                  rw data + free physical memory
//   0xfe000000..0: mapped direct (devices such as ioapic)
//
// The kernel allocates physical memory for its heap and for user memory
// between V2P(end) and the end of physical memory (PHYSTOP)
// (directly addressable from end..P2V(PHYSTOP)).

// This table defines the kernel's mappings, which are present in
// every process's page table.
static struct kmap {
  void *virt;
  uint phys_start;
  uint phys_end;
  int perm;
} kmap[] = {
 { (void*)KERNBASE, 0,             EXTMEM,    PTE_W}, // I/O space
 { (void*)KERNLINK, V2P(KERNLINK), V2P(data), 0},     // kern text+rodata
 { (void*)data,     V2P(data),     PHYSTOP,   PTE_W}, // kern data+memory
 { (void*)DEVSPACE, DEVSPACE,      0,         PTE_W}, // more devices
};

// Set up kernel part of a page table.
pde_t*
setupkvm(void)
{
  pde_t *pgdir;
  struct kmap *k;

  if((pgdir = (pde_t*)kalloc()) == 0)
    return 0;
  memset(pgdir, 0, PGSIZE);
  if (P2V(PHYSTOP) > (void*)DEVSPACE)
    panic("PHYSTOP too high");
  for(k = kmap; k < &kmap[NELEM(kmap)]; k++)
    if(mappages(pgdir, k->virt, k->phys_end - k->phys_start,
                (uint)k->phys_start, k->perm) < 0) {
      freevm(pgdir);
      return 0;
    }
  return pgdir;
}

// Allocate one page table for the machine for the kernel address
// space for scheduler processes.
void
kvmalloc(void)
{
  kpgdir = setupkvm();
  switchkvm();
}

// Switch h/w page table register to the kernel-only page table,
// for when no process is running.
void
switchkvm(void)
{
  lcr3(V2P(kpgdir));   // switch to the kernel page table
}

// Switch TSS and h/w page table to correspond to process p.
void
switchuvm(struct proc *p)
{
  if(p == 0)
    panic("switchuvm: no process");
  if(p->kstack == 0)
    panic("switchuvm: no kstack");
  if(p->pgdir == 0)
    panic("switchuvm: no pgdir");

  pushcli();
  mycpu()->gdt[SEG_TSS] = SEG16(STS_T32A, &mycpu()->ts,
                                sizeof(mycpu()->ts)-1, 0);
  mycpu()->gdt[SEG_TSS].s = 0;
  mycpu()->ts.ss0 = SEG_KDATA << 3;
  mycpu()->ts.esp0 = (uint)p->kstack + KSTACKSIZE;
  // setting IOPL=0 in eflags *and* iomb beyond the tss segment limit
  // forbids I/O instructions (e.g., inb and outb) from user space
  mycpu()->ts.iomb = (ushort) 0xFFFF;
  ltr(SEG_TSS << 3);
  lcr3(V2P(p->pgdir));  // switch to process's address space
  popcli();
}

// Load the initcode into address 0 of pgdir.
// sz must be less than a page.
void
inituvm(pde_t *pgdir, char *init, uint sz)
{
  char *mem;

  if(sz >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages(pgdir, 0, PGSIZE, V2P(mem), PTE_W|PTE_U);
  memmove(mem, init, sz);
}

// Load a program segment into pgdir.  addr must be page-aligned
// and the pages from addr to addr+sz must already be mapped.
int
loaduvm(pde_t *pgdir, char *addr, struct inode *ip, uint offset, uint sz)
{
  uint i, pa, n;
  pte_t *pte;

  if((uint) addr % PGSIZE != 0)
    panic("loaduvm: addr must be page aligned");
  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walkpgdir(pgdir, addr+i, 0)) == 0)
      panic("loaduvm: address should exist");
    pa = PTE_ADDR(*pte);
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(readi(ip, P2V(pa), offset+i, n) != n)
      return -1;
  }
  return 0;
}

// Allocate page tables and physical memory to grow process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
int
allocuvm(pde_t *pgdir, uint oldsz, uint newsz)
{
  char *mem;
  uint a;

  if(newsz >= KERNBASE)
    return 0;
  if(newsz < oldsz)
    return oldsz;

  a = PGROUNDUP(oldsz);
  for(; a < newsz; a += PGSIZE){
    mem = kalloc();
    if(mem == 0){
      cprintf("allocuvm out of memory\n");
      deallocuvm(pgdir, newsz, oldsz);
      return 0;
    }
    memset(mem, 0, PGSIZE);
    if(mappages(pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U) < 0){
      cprintf("allocuvm out of memory (2)\n");
      deallocuvm(pgdir, newsz, oldsz);
      kfree(mem);
      return 0;
    }
  }
  return newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
int
deallocuvm(pde_t *pgdir, uint oldsz, uint newsz)
{
  pte_t *pte;
  uint a, pa;

  if(newsz >= oldsz)
    return oldsz;

  a = PGROUNDUP(newsz);
  for(; a  < oldsz; a += PGSIZE){
    pte = walkpgdir(pgdir, (char*)a, 0);
    if(!pte)
      a = PGADDR(PDX(a) + 1, 0, 0) - PGSIZE;
    else if((*pte & PTE_P) != 0){
      pa = PTE_ADDR(*pte);
      if(pa == 0)
        panic("kfree");
      char *v = P2V(pa);
      kfree(v);
      *pte = 0;
    }
  }
  return newsz;
}

// Free a page table and all the physical memory pages
// in the user part.
void
freevm(pde_t *pgdir)
{
  uint i;

  if(pgdir == 0)
    panic("freevm: no pgdir");
  deallocuvm(pgdir, USER_UB, 0);
  for(i = 0; i < NPDENTRIES; i++){
    if(pgdir[i] & PTE_P){
      char * v = P2V(PTE_ADDR(pgdir[i]));
      kfree(v);
    }
  }
  kfree((char*)pgdir);
}

// Clear PTE_U on a page. Used to create an inaccessible
// page beneath the user stack.
void
clearpteu(pde_t *pgdir, char *uva)
{
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if(pte == 0)
    panic("clearpteu");
  *pte &= ~PTE_U;
}

// Given a parent process's page table, create a copy
// of it for a child.
pde_t*
copyuvm(pde_t *pgdir, uint sz)
{
  pde_t *d;
  pte_t *pte;
  uint pa, i, flags;
  char *mem;

  if((d = setupkvm()) == 0)
    return 0;
  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walkpgdir(pgdir, (void *) i, 0)) == 0)
      panic("copyuvm: pte should exist");
    if(!(*pte & PTE_P))
      panic("copyuvm: page not present");
    pa = PTE_ADDR(*pte);
    flags = PTE_FLAGS(*pte);
    if((mem = kalloc()) == 0)
      goto bad;
    memmove(mem, (char*)P2V(pa), PGSIZE);
    if(mappages(d, (void*)i, PGSIZE, V2P(mem), flags) < 0) {
      kfree(mem);
      goto bad;
    }
  }
  return d;

bad:
  freevm(d);
  return 0;
}

//PAGEBREAK!
// Map user virtual address to kernel address.
char*
uva2ka(pde_t *pgdir, char *uva)
{
  pte_t *pte;

  pte = walkpgdir(pgdir, uva, 0);
  if((*pte & PTE_P) == 0)
    return 0;
  if((*pte & PTE_U) == 0)
    return 0;
  return (char*)P2V(PTE_ADDR(*pte));
}

// Copy len bytes from p to user address va in page table pgdir.
// Most useful when pgdir is not the current page table.
// uva2ka ensures this only works for PTE_U pages.
int
copyout(pde_t *pgdir, uint va, void *p, uint len)
{
  char *buf, *pa0;
  uint n, va0;

  buf = (char*)p;
  while(len > 0){
    va0 = (uint)PGROUNDDOWN(va);
    pa0 = uva2ka(pgdir, (char*)va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (va - va0);
    if(n > len)
      n = len;
    memmove(pa0 + (va - va0), buf, n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
  }
  return 0;
}

int has_shm(int tab_idx[])
{
  int i, res = 0;
  for (i = 0; i < MAX_SHM_NUMBER_PER_PROCESS; ++i)
  {
    if (tab_idx[i] >= 0)
      ++res;
  }
  return res;
}

int free_shm(pde_t * pgdir, uint va[], int tab_idx[])
{
  int i;
  for (i = 0; i < MAX_SHM_NUMBER_PER_PROCESS; ++i)
  {
    if (tab_idx[i] >= 0 && shm_cnt[tab_idx[i]] == 1)
    {
      --shm_cnt[tab_idx[i]];
      shm_dealloc(pgdir, va[i]);
    }
    else if (tab_idx[i] >= 0 && shm_cnt[tab_idx[i]] > 1)
    {
      --shm_cnt[tab_idx[i]];
    }
  }
  return 0;
}

void incre_next_shm_tab()
{
  ++next_shm_tad_id;
  if (next_shm_tad_id >= MAX_SHM_NUMBER_TOTAL)
  {
    next_shm_tad_id = 0;
  }
}

int shm_copy(pde_t * src, uint shm_va[], int shmps_idx[], pde_t * dst)
{
  pde_t * pte;
  uint pa;
  int i;
  for (i = 0; i < MAX_SHM_NUMBER_PER_PROCESS; ++i)
  {
    if (shm_va[i])
    {
      pte = walkpgdir(src, (void *) shm_va[i], 0);
      if (!pte)
      {
        panic("shm_copy: pte should exist");
      }
      pa = PTE_ADDR(*pte);
      mappages(dst, (void *) shm_va[i], PGSIZE, /*V2P(shm_tab[shmps_idx[i]])*/pa, PTE_U | PTE_W);
      ++shm_cnt[shmps_idx[i]];
    }
  }
  return 1;
}

int shm_dealloc(pde_t * pgdir, uint shm_va)
{
  pde_t * pte = walkpgdir(pgdir, (void *) shm_va, 0);
  if (pte && (*pte & PTE_P) != 0)
  {
    uint pa = PTE_ADDR(*pte);
    kfree(P2V(pa));
    *pte = 0;
  }
  return 0;
}

int shm_alloc(pde_t * pgdir, uint old_shm, uint new_shm, int * used, int shms_idx[MAX_SHM_NUMBER_PER_PROCESS], uint shms_va[MAX_SHM_NUMBER_PER_PROCESS])
{
  if (old_shm & 0xfff || new_shm & 0xfff || old_shm < new_shm || old_shm > KERNBASE)
    return 0;
  int i;
  // check process shared memory usage
  int cnt = 0;  // number of shared memory used in process
  int next_p_id = -1;  // next free id in process
  for (i = 0; i < MAX_SHM_NUMBER_PER_PROCESS; ++i)
  {
    if (*used & (1 << i))
    {
      ++cnt;
    }
    else if (next_p_id < 0)
    {
      next_p_id = i;
    }
  }
  if (cnt >= MAX_SHM_NUMBER_PER_PROCESS)
  {
    cprintf("max shared memory number in process: %d\n", MAX_SHM_NUMBER_PER_PROCESS);
    return 0;
  }
  // check total available shared memory
  int next_tab_id = -1;
  for (i = 0; i < MAX_SHM_NUMBER_TOTAL; ++i)
  {
    if (shm_tab[next_shm_tad_id] == 0)
    {
      next_tab_id = next_shm_tad_id;
      incre_next_shm_tab();
      break;
    }
    incre_next_shm_tab();
  }
  if (next_tab_id < 0)
  {
    cprintf("max shared memory: %d\n", MAX_SHM_NUMBER_TOTAL);
    return 0;
  }

  char * mem = kalloc();
  if (mem == 0)
  {
    cprintf("kalloc failed\n");
    return 0;
  }
  memset(mem, 0, PGSIZE);
  mappages(pgdir, (void *) new_shm, PGSIZE, V2P(mem), PTE_U | PTE_W);

  shms_va[next_p_id] = new_shm;
  shms_idx[next_p_id] = next_tab_id;
  shm_tab[next_tab_id] = (uint) mem;
  shm_cnt[next_tab_id] = 1;
  *used |= (1 << next_p_id);
  return new_shm;
}

void* spalloc()
{
  struct proc * curproc = myproc();
  pde_t * pgdir = curproc->pgdir;
  uint shm_lb = curproc->shm_lb;
  int shm_used = curproc->shm_used;
  int res = shm_alloc(pgdir, shm_lb, shm_lb - PGSIZE, &shm_used, curproc->shms_idx, curproc->shms_va);
  if (res == 0)
  {
    cprintf("spalloc failed\n");
    return 0;
  }
  shm_lb -= PGSIZE;
  curproc->shm_lb = shm_lb;
  curproc->shm_used = shm_used;
  return (void *) res;
}

int spfree(uint va)
{
  int idx = -1, i;
  struct proc * curproc = myproc();
  for (i = 0; i < MAX_SHM_NUMBER_PER_PROCESS; ++i)
  {
    if (curproc->shms_va[i] == va)
    {
      idx = i;
    }
  }
  if (idx < 0)
    return -1;
  int tab_idx = curproc->shms_idx[idx];
  curproc->shm_used &= ~(1 << idx);
  curproc->shms_va[idx] = 0;
  curproc->shms_idx[idx] = -1;
  if (--shm_cnt[tab_idx] == 0)
  {
    shm_dealloc(curproc->pgdir, va);
  }
  return 0;
}

//PAGEBREAK!
// Blank page.
//PAGEBREAK!
// Blank page.
//PAGEBREAK!
// Blank page.

