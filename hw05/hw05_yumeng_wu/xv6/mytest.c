#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char* _av[])
{
  int i = 0;
  int * ii = &i;
  exit1(i);
  wait1(ii);
  exit();
}
