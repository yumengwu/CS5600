#include "types.h"
#include "stat.h"
#include "user.h"

void
copy(char* dst, const char* src)
{
  memmove(dst, src, strlen(src) + 1);
}

int
main(int _ac, char* _av[])
{
  mutex_t* mx = spalloc();
  mutex_init(mx);

  char* t1 = "Alice";
  //char* t2 = "Bob";

  char* s1 = spalloc();
  //printf(1, "Allocated memory: %p\n", s1);
  copy(s1, t1);

  int cpid;
  if ((cpid = fork())) {
    mutex_lock(mx);
    printf(1, "p1: Hi, %s\n", s1);
    mutex_unlock(mx);

    mutex_lock(mx);
    printf(1, "p2: Hi, %s\n", s1);
    mutex_unlock(mx);

    wait();
    spfree(s1);
    printf(1, "\ntest3 done\n");
  }
  else {
    mutex_lock(mx);
    printf(1, "c1: Hi, %s\n", s1);
    mutex_unlock(mx);

    mutex_lock(mx);
    printf(1, "c2: Hi, %s\n", s1);
    mutex_unlock(mx);

    spfree(s1);
  }

  exit();
}
