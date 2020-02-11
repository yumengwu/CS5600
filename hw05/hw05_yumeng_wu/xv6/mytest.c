#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char* _av[])
{
  int i = 123;
  int * ii = &i;
  wait1(ii);
  printf(1, "before exit1\n");
  exit1(i);
  printf(1, "after exit1\n");
  exit();
}
