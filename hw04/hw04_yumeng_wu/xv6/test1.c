#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int _ac, char *_av[])
{
  struct iostats st;
  int rv;
  int fd1, fd2;
  char buf[128];

  fd1 = open("test11.txt", O_CREATE | O_WRONLY);
  fd2 = open("test12.txt", O_CREATE | O_WRONLY);

  for (int ii = 0; ii < 10; ++ii) {
    write(fd1, "1234", 4);
  }

  for (int ii = 0; ii < 11; ++ii) {
    write(fd2, "abc", 3);
  }

  rv = getiostats(fd1, &st);
  printf(1, "fd1 stats(%d) (r: %d, w: %d) -> %d\n", fd1, st.read_bytes, st.write_bytes, rv);

  rv = getiostats(fd2, &st);
  printf(1, "fd2 stats(%d) (r: %d, w: %d) -> %d\n", fd2, st.read_bytes, st.write_bytes, rv);

  for (int ii = 0; ii < 8; ++ii) {
    write(fd2, "ab", 2);
  }

  rv = getiostats(fd2, &st);
  printf(1, "fd2a stats(%d) (r: %d, w: %d) -> %d\n", fd2, st.read_bytes, st.write_bytes, rv);

  close(fd2);
  close(fd1);

  fd1 = open("test11.txt", O_RDONLY);
  fd2 = open("test12.txt", O_RDONLY);

  read(fd1, buf, 10);
  read(fd2, buf, 128);

  rv = getiostats(fd1, &st);
  printf(1, "fd1r stats(%d) (r: %d, w: %d) -> %d\n", fd1, st.read_bytes, st.write_bytes, rv);

  rv = getiostats(fd2, &st);
  printf(1, "fd2r stats(%d) (r: %d, w: %d) -> %d\n", fd2, st.read_bytes, st.write_bytes, rv);

  read(fd1, buf, 128);

  rv = getiostats(fd1, &st);
  printf(1, "fd1ra stats(%d) (r: %d, w: %d) -> %d\n", fd1, st.read_bytes, st.write_bytes, rv);

  close(fd2);
  close(fd1);

  printf(1, "test1 done\n");
  exit();
  return 0;
}
