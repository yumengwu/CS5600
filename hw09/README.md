## Description

## Garbage Collector for C

For this assignment you’ll be completing another memory allocator, this time an automatic mark and sweep garbage collector.

This time our memory allocators will consist of four functions (or macros):

(macro) GC_INIT();<br/>
void* gc_malloc(size_t bytes);<br/>
void gc_collect();<br/>
void gc_print_stats();<br/>

- <font color="#e83e8c">GC_INIT()</font> must be called first thing in main() to initialize the allocator.
- <font color="#e83e8c">gc_malloc</font> allocates memory by finding an unused region of memory of the appropriate size and returning a pointer to the beginning of that region. This may involve doing a collection.
- <font color="#e83e8c">gc_collect</font> forces a full garbage collection run to occur immediately, freeing as much memory as possible.
- <font color="#e83e8c">gc_print_stats</font> prints out some stats on your garbage collector.

## What to do.

Complete the starter code so that you pass the tests.

This will require you to:

- First, fix gc_malloc to allocate as usual for a free-list allocator. This will get you passing about half the tests.
- Implement the appropriate logic for a mark and sweep collector.
- Correctly track the stats for gc_print_stats. The code for this is mostly in place, but make sure you update the stats appropriately in the functions you modify.

You should maintain the basic design of the starter code, including a fixed 1MB garbage collected heap.

## To Submit:

- A tarball containing the starter code and your modifications .
- There should be a single top level directory containing the same structure as provided in the starter code.
- Appropriate permissions should be maintained on the submitted files.
- Make sure you don’t change the test script, test code, or top level Makefile.
- No executables or object files.
- Make extra sure to run “make clean” before submitting.
- No extra hidden files.
