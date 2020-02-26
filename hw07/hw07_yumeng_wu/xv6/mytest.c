#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char* _av[])
{
  // int i;
  char * str = spalloc();
  printf(1, "%d\n", str);
  memset(str, 0, 100);
  memset(str, 'a', 10);
  printf(1, "%s\n", str);
  // for (i = 0; i < 26; ++i)
  // {
  //   str[i] = 'a' + i;
  // }
  // printf(1, "%s\n", str);
  spfree(0);
  return 0;
}
