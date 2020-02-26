#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char* argv[])
{
  int cpid, status;
  if ((cpid = fork())) {
    wait1(&status);
    printf(1, "status = %d\n", status);
  }
  else {
    exec(argv[1], argv + 1);
  }

  exit1(0);
  return 0;
}
