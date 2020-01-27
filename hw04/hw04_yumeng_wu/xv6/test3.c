#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char *_av[])
{
  int rv, rv0;
  struct iostats st;
  struct iostats st0;

  printf(2, "Hello\n");
  rv = getiostats(2, &st);
  printf(1, "hello1 stats(2) (r: %d, w: %d) -> %d\n", st.read_bytes, st.write_bytes, rv);

  rv0 = getiostats(2, &st0);

  printf(2, "Hello\n");
  rv = getiostats(2, &st);

  printf(1, "base stats0(2) (r: %d, w: %d) -> %d\n", st0.read_bytes, st0.write_bytes, rv0);
  printf(1, "raw stats(2) (r: %d, w: %d) -> %d\n", st.read_bytes, st.write_bytes, rv);
  printf(1, "diff stats(2) (r: %d, w: %d) -> %d\n",
         st.read_bytes - st0.read_bytes,
         st.write_bytes - st0.write_bytes,
         rv0 + rv);

  printf(1, "test3 done\n");
  exit();
  return 0;
}
