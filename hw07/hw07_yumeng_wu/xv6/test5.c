#include "types.h"
#include "stat.h"
#include "user.h"

volatile int zz = 0;

int
main(int _ac, char* _av[])
{
  long* count = spalloc();
  mutex_t* mx = spalloc();
  mutex_init(mx);

  int kids[10];
  *count = 0;

  for (int pp = 0; pp < 10; ++pp) {
    if ((kids[pp] = fork())) {
      // skip
    }
    else {
      for (int ii = 0; ii < 100; ++ii) {
        mutex_lock(mx);
        long temp = *count + 1;
        long ww = pp * 500 + ii;
        for (int jj = 0; jj < ww; ++jj, ++zz);
        *count = temp;
        mutex_unlock(mx);
      }
      sleep(500);

      mutex_lock(mx);
      printf(1, "p%d: count = %d\n", pp, *count);
      mutex_unlock(mx);

      spfree(count);
      spfree(mx);
      exit();
    }
  }

  for (int pp = 0; pp < 10; ++pp) {
    wait();
  }

  printf(1, "final count = %d\n", *count);
  printf(1, "test5 done\n");

  spfree(count);
  spfree(mx);
  exit();
}
