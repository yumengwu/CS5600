## Description

## The assignment

For this assignment you’ll be adding improved shell functionality to xv6. This will require both modifications to the shell program (sh.c) and the OS overall.

## Exit Codes

Add two new syscalls:

int exit1(int status);
int wait1(int* status);

These should act like the existing exit and wait syscalls, except passing an exit status from the child back to the waiting parent.

We’re adding system calls rather than modifying the existing ones in order to provide backwards compatiblity with the existing xv6 programs rather than needing to modify everything.

The existing exit syscall should be modified so that a wait1 on that process will return a success (0) exit status.

Once you’ve completed these modifications, you should add the following binaries to the UPROGS list in the Makefile: true, false, status, done, retlen

## Add Features to the xv6 shell

Add the following features to the xv6 shell (in sh.c):

Feature 1: Shell scripts.

- If “sh” is run with a single command line argument, it should open that file and read commands from there instead of from the console.
- When reading from a script, the shell should not print “$” prompts.
- If the script file can’t be opened, the shell should exit with a non-zero status.

Feature 2: && and || operators.

- These operators operate much like the “;” sequence operator, except the execution of the subcommand on the left is dependent on the exit status of the subcommand on the right.
- A command “foo && bar” will execute bar only if foo succeeds (exits with status 0).
- A command “foo || bar” will execute bar only if foo fails (exits with a non-zero status).
- These operators should be added in a manner consistent with the design of existing operators in the xv6 shell.

## Allow Returns from main

This feature may be somewhat tricky to implement. It’s recommended to get everything else working first.

Modify xv6 so that programs can return a value from main to report their status. This should act like the exit1 syscall with the same value, and should not result in any unhandled traps on exit.

## To Submit:

- A tarball containing the starter code and your modifications.
- There should be a single top level directory containing the same structure as provided in the starter code.
- Make sure you don’t change the test script, test executables, or top level Makefile.
- No executables or object files.
- Make extra sure to run “make clean” before submitting.
- No extra hidden files.
