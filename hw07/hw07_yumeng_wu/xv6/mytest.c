#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char* _av[])
{
  char * s1 = spalloc();
  char * s2 = spalloc();
  char * s3 = spalloc();
  memset(s1, 'a', 10);
  memset(s2, 'x', 10);
  memset(s3, 'y', 10);
  printf(1, "before fork: %s\n", s1);
  if (fork() == 0)
  {
    char * s4 = spalloc();
    memset(s1, 'b', 5);
    memset(s4, 'h', 20);
    printf(1, "child: %s\n", s1);
    sleep(100);
    printf(1, "child: %s\n", s1);
    memset(s1, 'd', 1);
    printf(1, "child s4: %s\n", s4);
    spfree(s2);
    spfree(s4);
    exit1(1);
  }
  sleep(50);
  memset(s1, 'c', 3);
  printf(1, "parent: %s\n", s1);
  wait();
  printf(1, "parent: %s\n", s1);
  spfree(s1);
  spfree(s2);
  spfree(s3);
  return 0;
}
