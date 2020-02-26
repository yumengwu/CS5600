#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char* _av[])
{
  const int TOP = 100;

  long long* count = spalloc();
  long long temp;
  mutex_t* mx = spalloc();
  mutex_init(mx);

  int cpid;
  if ((cpid = fork())) {
    for (int ii = 0; ii < TOP; ++ii) {
      mutex_lock(mx);
      temp = *count;
      sleep(1);
      *count = temp + 1;
      mutex_unlock(mx);
    }

    wait();

    printf(1, "count = %d\n", *count);
    printf(1, "test4 done\n");

    spfree(count);
    spfree(mx);
    exit();
  }
  else {
    for (int ii = 0; ii < TOP; ++ii) {
      mutex_lock(mx);
      temp = *count;
      sleep(1);
      *count = temp + 1;
      mutex_unlock(mx);
    }

    spfree(count);
    spfree(mx);
    exit();
  }
}
