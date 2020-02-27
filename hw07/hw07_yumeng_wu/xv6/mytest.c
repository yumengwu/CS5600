#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int _ac, char* _av[])
{
  mutex_t* mutex = spalloc();
  mutex_init(mutex);
  char * s1 = spalloc();
  memset(s1, 'a', 20);
  printf(1, "before %s\n", s1);
  if (fork() == 0)
  {
    sleep(10);
    mutex_lock(mutex);
    memset(s1, 'b', 5);
    printf(1, "child: %s\n", s1);
    mutex_unlock(mutex);
    sleep(100);
    exit();
  }
  mutex_lock(mutex);
  memset(s1, 'c', 8);
  sleep(400);
  printf(1, "parent: %s\n", s1);
  mutex_unlock(mutex);
  wait();
  return 0;
}
