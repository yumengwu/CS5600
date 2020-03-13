/*
#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
*/
#include <stdio.h>
#include "xmalloc.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.
//
// Then copied from xv6.

// TODO: Remove this stuff
// typedef unsigned long uint;
// static char* sbrk(uint nn) { return 0; }
// TODO: end of stuff to remove


typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

// TODO: This is shared global data.
// You're going to want a mutex to protect this.
static Header base;
static Header *freep;

// static int * sem_id_m = 0, * sem_id_f = 0, * sem_id_r = 0;
static pthread_mutex_t mutex_m, mutex_f, mutex_r;
static int init_m = 0, init_f = 0, init_r = 0;

// void semwait(int * semp)
// {
//   struct sembuf sem_b;
//   sem_b.sem_num = 0;
// 	sem_b.sem_op = -1;
// 	sem_b.sem_flg = SEM_UNDO;
//   semop(*semp, &sem_b, 1);
// }

// void sempost(int * semp)
// {
//   struct sembuf sem_b;
//   sem_b.sem_num = 0;
// 	sem_b.sem_op = 1;
// 	sem_b.sem_flg = SEM_UNDO;
//   semop(*semp, &sem_b, 1);
// }

void
xfree(void *ap)
{
  // if (sem_id_f == 0) {
  //   sem_id_f = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  //   *sem_id_f = semget((key_t) 1234, 1, IPC_CREAT);
  // }
  // semwait(sem_id_f);
  if (!init_f) {
    pthread_mutex_init(&mutex_f, 0);
    init_f = 1;
  }
  pthread_mutex_lock(&mutex_f);
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  // if(bp + bp->s.size == p->s.ptr){
  //   bp->s.size += p->s.ptr->s.size;
  //   bp->s.ptr = p->s.ptr->s.ptr;
  // } else
  //   bp->s.ptr = p->s.ptr;
  // if(p + p->s.size == bp){
  //   p->s.size += bp->s.size;
  //   p->s.ptr = bp->s.ptr;
  // } else
  //   p->s.ptr = bp;
  // freep = p;
  // sempost(sem_id_f);
  pthread_mutex_unlock(&mutex_f);
  // printf("p %d %d\n", p->s.ptr, p->s.size);
  // printf("xfree finish\n");
}

static Header*
morecore(uint nu)
{
  char *ap;
  Header *hp;
  nu += sizeof(Header);
  if(nu < 4096)
    nu = 4096;
  // TODO: Replace sbrk use with mmap
  // printf("%ld %ld\n", nu, nu * sizeof(Header));
  // p = sbrk(nu * sizeof(Header));
  ap = mmap(0, nu, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  if(ap == (char*)0)
    return 0;
  hp = (Header*)ap;
  hp->s.size = nu - sizeof(Header);
  // printf("hp %d %d %d\n", ap, hp->s.ptr, hp->s.size);
  // xfree((void*)(hp + 1));
  Header *bp, *p;
  bp = (Header*)ap;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
  // printf("freep %d %d\n", freep->s.ptr, freep->s.size);
  return freep;
}

void*
xmalloc(size_t nbytes)
{
  // if (sem_id_m == 0) {
  //   sem_id_m = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  //   *sem_id_m = semget((key_t) 4321, 1, IPC_CREAT);
  // }
  // printf("sem id: %d\n", *sem_id_m);
  // semwait(sem_id_m);
  if (!init_m) {
    pthread_mutex_init(&mutex_m, 0);
    init_m = 1;
  }
  pthread_mutex_lock(&mutex_m);
  Header *p, *prevp;
  uint nunits;

  // nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  nunits = nbytes;
  // printf("nunits %d\n", nunits);
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    // printf("p %d %d %d\n", p, p->s.ptr, p->s.size);
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        // printf("p size: %d\n", p->s.size);
        p->s.size -= nunits;
        p = (void *) p + p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      // sempost(sem_id_m);
      pthread_mutex_unlock(&mutex_m);
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0){
        // sempost(sem_id_m);
        pthread_mutex_unlock(&mutex_m);
        return 0;
      }
  }
}

void*
xrealloc(void* prev, size_t nn)
{
  // TODO: Actually build realloc.
  // if (sem_id_r == 0) {
  //   sem_id_r = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  //   *sem_id_r = semget((key_t) 2333, 1, IPC_CREAT);
  // }
  // semwait(sem_id_r);
  if (!init_r) {
    pthread_mutex_init(&mutex_r, 0);
    init_r = 1;
  }
  xfree(prev);
  void * res = xmalloc(nn);
  // sempost(sem_id_r);
  pthread_mutex_unlock(&mutex_r);
  return res;
}
