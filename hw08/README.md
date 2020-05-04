## Description

## Optimized Memory Allocator

For this assignment you’ll be building an optimized memory allocator.

Specifically, our memory allocators will consist of three functions:

void* malloc(size_t bytes);<br/>
void free(void* ptr);<br/>
void* realloc(void* ptr, size_t size);<br/>

- <font color="#e83e8c">malloc</font> allocates memory by finding an unused region of memory of the appropriate size and returning a pointer to the beginning of that region.
- <font color="#e83e8c">free</font> takes a pointer returned by malloc and returns it to the pool of unused memory.
- <font color="#e83e8c">realloc</font> resizes an existing allocation to make it bigger, either by resizing it in place or by allocating a larger region of memory and copying over the data from the old allocation.

In order to avoid conflicts with the existing system allocators, we’ll be implementing functions to the interface in “xmalloc.h”: xmalloc, xfree, and xrealloc.

### Part 1: Port the xv6 allocator to Linux

The starter code includes the memory allocator from xv6 with some minor modificaitons to match the xmalloc interface and to get it to compile at all.

This allocator appears in “xv6_malloc.c”.

You need to port this allocator to Linux. To do this:

- Replace any use of the “sbrk” system call with a call to “mmap”.
- Make the allocator thread safe by adding a mutex to protect the shared global state (i.e. the free list).
- Make sure the types all make sense for AMD64 Linux.
- Look for any TODO comments and do that stuff, like implementing realloc.

Don’t further modify or optimize this allocator, since you’ll be using it as a baseline for a performance comparision with the system allocator and your optimized allocator.

### Part 2: Build a new, optimized allocator

Once your xv6 allocator is working, you’ll have two allocators: The ported xv6 allocator and the system allocator from GNU libc.

Next, implement a new optimized allocator in the “opt_malloc.c” source file. This can be based on the ported xv6 allocator, but it’s recommended to just write a new allocator from scratch.

For this allocator:

- Memory should be requested from the OS using mmap and then used to satisfy malloc, realloc, and free requests as appropriate.
- You should use mechanisms like those discussed in class (e.g. free list, buddy system, buckets, multiple arenas, thread-local storage) to speed up the allocator.
- The maximum amount of memory provided to satisfy a malloc call shouldn’t be more than 3x the requested amount.
- Some mechanism should be in place so that freed memory is either returned to the operating system (with munmap) or eventually reused.
- Whenever an entire mmaped block of memory is freed that memory should be available for reuse for any future allocation.
- Both test programs (collatz-list and collatz-ivec) should be significantly faster (e.g. at least 4x faster) with your optimized allocator than the xv6 allocator on sufficiently large input sizes.
- Both test programs should be consistently faster (e.g. more than 10% faster) with your optimized allocator than the system allocator on sufficiently large input sizes.
- The allocator should work for arbitrary programs even if it’s optimized for the test cases.

Note that execution time on the Inkfish server is an intended component of some of the automated tests.

### Part 3: Benchmark + Report + Graph

Once your allocators are working, the next step is to test the performance.

The provided Makefile builds six binaries for the Collatz program. For each of the three allocators (system, xv6, optimzied) it builds a version that uses linked lists and one that uses vectors. These programs take an integer N as a command line argument search the integers 1..N for the longest Collatz sequence. Running these test programs with larger input should allocate more memory and take more time.

First, for each collatz test program, determine what the highest power of 10 input your computer can execute in 10 seconds (e.g. try 10, 100, 1000, …). Do some additional tests and then create a graph, graph.png, showing the highest input value (to two significant digits) that your computer can execute in 10 seconds.

Write a report that answers the following questions:

- Compare your optimized allocator to the xv6 allocator. For each of (list, ivec), pick an input size where the slower allocator takes about 10 seconds. How fast did each allocator run for that input? Which one was faster and by how much? Show measurements with a table.
- Compare your optimized allocator to the system allocator. For each of (list, ivec), pick an input size where the slower allocator takes about 10 seconds. How fast did each allocator run for that input? Which one was faster and by how much? Show measurements with a table.
- What techniques did you use in your optimized allocator.
- How does your allocator handle reusing memory?
- What was the most significant challenge in building your optimized allocator.
- If you were to redo the assignment would you use the same allocator design? Why?

Your report should be named report.txt and formatted as a UTF-8 UNIX text file wrapped at 70 columns.

## Your report should be named report.txt and formatted as a UTF-8 UNIX text file wrapped at 70 columns.

- A tarball containing the starter code and your modifications as well as a report.txt and graph.png.
- There should be a single top level directory containing the same structure as provided in the starter code.
- Appropriate permissions should be maintained on the submitted files.
- Make sure you don’t change the test script, test code, or top level Makefile.
- No executables or object files.
- Make extra sure to run “make clean” before submitting.
- No extra hidden files.
