## Description

## The assignment

For this assignment you’ll be adding shared memory to xv6.

To do this, you’ll add five functions to the userspace API of xv6:

\# shared memory<br/>
void* spalloc();<br/>
void spfree(void* ptr);<br/>

\# mutexes<br/>
int mutex_init(mutex_t* lock);<br/>
int mutex_lock(mutex_t* lock);<br/>
int mutex_unlock(mutex_t* lock);<br/>

Shared Memory:

- The spalloc function should allocate a single 4k page of shared memory and return a pointer to it.
- Shared memory should remain shared across calls to fork().
- The physical page associated with a shared page should be freed when every process that had a reference to it has freed it (either by calling spfree on the pointer, by calling exit, or by calling exec).
- You don’t need to support more than 10 shared pages per process or 100 shared pages total.
- spalloc and spfree may be system calls, or they may may use new or existing system calls internally.

Mutexes:

- mutex_init should be run once to initialize a mutex
- all three mutex functions return 0 on success
- Implement your lock in userspace using atomic instructions. See chapter 28 of OSTEP for ideas and see the [atomic builtins](https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html) in the GCC manual.
- The implementation of mutexes should not involve any system calls.
- You may need to change the scheduler from “run the lowest number process first” to a more round-robin solution (i.e. run the next numbered process, remembering previous position) to allow progress with userspace mutexes.

System calls can be added as in previous assignments. Userspace standard library functions should have prototypes added to the existing library headers (e.g. user.h, types.h) and can either be implemented in an existing file like ulib.c or malloc.c or in a new source file built into all userspace programs similarly to ulibc.c and malloc.c.

## To Submit:

- A tarball containing the starter code and your modifications.
- There should be a single top level directory containing the same structure as provided in the starter code.
- Appropriate permissions should be maintained on the submitted files.
- Make sure you don’t change the test script, test code, or top level Makefile.
- No executables or object files.
- Make extra sure to run “make clean” before submitting.
- No extra hidden files.
