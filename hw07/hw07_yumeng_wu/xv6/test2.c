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
  char* t1 = "Alice";
  char* t2 = "Bob";

  char* s1 = spalloc();
  //printf(1, "Allocated memory: %p\n", s1);
  copy(s1, t1);

  int cpid;
  if ((cpid = fork())) {
    sleep(5);
    printf(1, "p1: Hi, %s\n", s1);
    copy(s1, t2);
    //printf(1, "p: %p\n", s1);
    printf(1, "p2: Hi, %s\n", s1);
    wait();
    printf(1, "p3: Hi, %s\n", s1);
    spfree(s1);
    printf(1, "\ntest2 done\n");
  }
  else {
    //printf(1, "c: %p\n", s1);
    printf(1, "c1: Hi, %s\n", s1);
    sleep(20);
    //printf(1, "c: %p\n", s1);
    printf(1, "c2: Hi, %s\n", s1);
    spfree(s1);
  }

  exit();
}
