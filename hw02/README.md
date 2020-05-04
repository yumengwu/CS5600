## Description

You are expected to write the assembly code for for assembly assignments in this class. Compiler output won’t be accepted, even if you’ve edited it.

## Task 1: Fibonacci Numbers

The Fibonacci function is defined as:
- fib(0) = 0
- fib(1) = 1
- fib(x) = fib(x-1) + fib(x-2) when x > 1

Write program in C that calcuates the x-th Fibonacci number defined above and prints the result as shown in the interactions below. You should use the naive implementation with a fib function that makes two recursive calls. Invalid input should print a usage message.

Once you’ve completed the C program, rewrite it in both AMD64 assembly and i386 assembly. Your assembly programs should have the same functions as the C program, and all three programs should function identically.

Sample interactions:

\$ ./fib64 0<br/>
fib(0) = 0<br/>
\$ ./cfib 10<br/>
fib(10) = 55<br/>
\$ ./fib32 10<br/>
fib(10) = 55<br/>
\$ ./fib64<br/>
Usage: ./fib N, where N >= 0<br/>
\$ ./cfib -4<br/>
Usage: ./fib N, where N >= 0<br/>
\$<br/>

Each function in your assembly program should follow the conventions discussed in class. Include variable mapping comments in your assembly: for each variable in the C code, where is that value stored in the assembly code?

## Task 2: Command Line Calculator

Write both a C program that take a simple arithmetic expression on the command line and print the result formatted as shown in the interactions below.

Once you have a working C program, write versions in amd64 and i386 assembly. The three programs should function identically.

You should support the the following arithemetic operations on long integers: (+, -, *, /). Invalid input should print a usage message.

\$ ./calc32 1 + 4<br/>
1 + 4 = 5<br/>
\$ ./ccalc 2 "*" 7<br/>
2 * 7 = 14<br/>
\$ ./calc64 2 / 70<br/>
2 / 70 = 0<br/>
\$ ./ccalc<br/>
Usage:<br/>
  ./ccalc N op N<br/>
\$ ./calc32 2 AND 7<br/>
Usage:<br/>
  ./acalc N op N<br/>

Note: You need to quote “*” to make the shell happy. The argument is still just a single character.

Each function in your assembly program should follow the conventions discussed in class, including variable mapping comments.

## To Submit:
- A tarball containing the starter code and your modifications.
- That should be a single directory containing your source code; feel free to rename the directory.
- Make sure you don’t change the test script or Makefile.
- No binaries or object files.
- No extra hidden files.
