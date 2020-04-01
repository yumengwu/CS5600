#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char* argv[])
{
  if (argc != 2) {
    printf(1, "\nusage: %s TAG\n", argv[0]);
    exit1(1);
  }
  printf(1, "\ntest complete: %s\n", argv[1]);
  exit1(0);
}
