#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char* _av[])
{
  int pid = fork();
  if (pid == 0) {
    return 12;
  }
  int a = 0;
  printf(1, "child pid: %d\n", pid);
  printf(1, "pid %d\n", waitpid(pid, &a));
  printf(1, "status %d\n", a);
  sleep(10);
  return 0;
}
