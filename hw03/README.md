## Description

You are expected to write the assembly code for for assembly assignments in this class. Compiler output won’t be accepted, even if you’ve edited it.

You have two weeks for this assignment. You’ll want to plan to complete it early - the next assignment may also be difficult, and there will likely not be two weeks for that one.

## Sort some integers

For this assignment, you will write a program (in C, then AMD64 and i386 assembly) to sort a collection of (32-bit) integers.

Your program will take two command line arguments. The first argument is the input file, the second is the output file.

The file format is just the integers stored as binary data. The first four bytes is the first integer, the next four bytes are the second integer, etc.

Perl scripts are provided in the starter code under the “tools” directory to create, display, and compare these files.

Example session:

\$ tools/gen-random.pl 4 nums.dat<br/>
\$ tools/print-file.pl nums.dat<br/>
35<br/>
8<br/>
-3<br/>
107<br/>
\$ ./sort nums.dat<br/>
Usage: ./sort input-file output-file<br/>
\$ ./sort nums.dat sorted.dat<br/>
\$ tools/print-file.pl sorted.dat<br/>
-3<br/>
8<br/>
35<br/>
107<br/>

Implementation requirements:
- Sorting should be done in a dedicated function that takes a pointer to the array of ints and its length and then performs the sort in place.
    - e.g. void sort(int* xs, long nn)
- The space for the array of integers should be allocated on the stack.
- You should not over-allocate space for the array of integers by more than a factor of two.
- You should not use functions from the standard C library. Write your own sort function, and use syscalls for I/O.
    - In C, this means syscall wrappers documented in section 2 of the manual.
    - Yes, that means you probaby want to implement strlen.
    - You can use the function perror(3) or strerror(3) to find and show error messages if a syscall fails (returns -1).
- For the assembly, follow the standard and class conventions (calling conventions, enter/leave, variable mapping comments, etc).
- Your assembly code should follow the structure (functions, statements) of your C code.
- On error, your program should exit with exit code 1.

Recommendations:

- I recommend [insertion sort](https://en.wikipedia.org/wiki/Insertion_sort) or [gnome sort](https://en.wikipedia.org/wiki/Gnome_sort) for your sort function.
- To create the output file, you’ll need to use the 3 argument version of open. A good value for mode is 0644.
- The system call to exit early is _exit(2). This shouldn’t be used in C programs that use the standard library. In a normal program you’d almost always use exit(3).
- Options to correctly size the array:
    - Scan the file beforehand to find its size.
    - Have a sort_file function which takes a size guess as input and fails if it runs out of space. If it fails, retry with a larger guess.
    - Use the fstat(2) or stat(2) syscall to find the file size. This may require some experimentation (e.g. C code) to find the size and layout of the stat structure for use from assembly.
- This is more complex than previous assignments.
    - Start early.
    - As soon as you have anything working, submit it.
    - Build smaller pieces first and get them working before attempting the entire task.
    - Save intermediate, partially working versions of your code before adding new functionality. You can use something like git - including with a private repo on github or gitlab - or just copy the whole directory someplace safe.
    - Make sure your intermediate backups are backed up off your VM in case you have a config that likes to break VM images.
    - Ignoring the requirements above just costs points. A version that uses standard C library functions (even qsort(3)) and always allocates a 1MB array is still worth partial credit. More requirements met means more credit.
    - Ignored requirements costs point per program.
- Example smaller pieces to build:
    - Reimplement tools/print-file.pl in C and Assembly.
    - Add a sort function, print sorted numbers to stdout with printf.
    - Once that works, write the output file as specified.
    - Make these smaller pieces for each version (C, AMD64, i386).

## Allocating space on the stack

In C, allocating a dynamic amount of space on the stack is simple: you can just declare a local array variable.

&emsp;&emsp;int nn = // calculate size;<br/>
&emsp;&emsp;int data[nn];

In assembly it’s also simple. You can just move the stack pointer down to give yourself more space. Note that by moving the stack pointer this moves any other stack-pointer-relative allocations you’ve done.

There is one complication: moving the stack pointer down by an unknown amount can unalign the stack pointer.

&emsp;&emsp;// calculate size in %rdx<br/>
&emsp;&emsp;sub %rdx, %rsp<br/>
&emsp;&emsp;lea 0(%rsp), %r12 <br/>
&emsp;&emsp;// %r12 now contains the address of your newly allocated stack space<br/>
&emsp;&emsp;// It's the "data" variable from the C code above.<br/>
    
&emsp;&emsp;// realign stack<br/>
&emsp;&emsp;mov $15, %r10<br/>
&emsp;&emsp;not %r10<br/>
&emsp;&emsp;and %r10, %rsp<br/>

## Extra Credit

You get extra credit (+9 on manual grading) if all three of your programs work and and use [merge sort](https://en.wikipedia.org/wiki/Merge_sort). Note this clearly in the code, and mention it in your submission note on Inkfish. Your sort function should still externally appear to sort in place (e.g. copy output back over input when done sorting).

## To Submit:
- A tarball containing the starter code and your modifications.
- That should be a single directory containing your source code; feel free to rename the directory.
- Make sure you don’t change the test script or Makefile.
- No executables or object files.
- No extra hidden files.
