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

  char* s1 = malloc(10);
  copy(s1, t1);

  int cpid;
  if ((cpid = fork())) {
    printf(1, "p: Hi, %s\n", s1);
    copy(s1, t2);
    printf(1, "p: Hi, %s\n", s1);
    wait();
    printf(1, "\ntest1 done\n");
  }
  else {
    sleep(20);
    printf(1, "c: Hi, %s\n", s1);
  }

  free(s1);
  exit();
}
