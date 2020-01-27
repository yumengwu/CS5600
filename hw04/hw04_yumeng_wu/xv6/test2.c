#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char *_av[])
{
  int rv0, rv1, rv2;
  struct iostats st0;
  struct iostats st1;
  struct iostats st2;

  rv0 = getiostats(0, &st0);
  rv1 = getiostats(1, &st1);
  rv2 = getiostats(2, &st2);

  printf(1, "stats before any output\n");
  printf(1, "stdin stats(0) (r: %d, w: %d) -> %d\n", st0.read_bytes, st0.write_bytes, rv0);
  printf(1, "stdout stats(1) (r: %d, w: %d) -> %d\n", st1.read_bytes, st1.write_bytes, rv1);
  printf(1, "stderr stats(2) (r: %d, w: %d) -> %d\n", st2.read_bytes, st2.write_bytes, rv2);
  printf(1, "test2 done\n");
  exit();
  return 0;
}
