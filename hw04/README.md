## Description

## First, install some prereqs

sudo apt install libexpect-perl qemu-system-x86 qemu-utils

## The assignment

For this assignment you’ll be adding a system call:

struct iostats {
&emsp;&emsp;uint read_bytes;
&emsp;&emsp;uint write_bytes;
};

int getiostats(int fd, struct iostats* stats);
Given a file descriptor, getiostats gets the total number of bytes read and written on that file descriptor since it was opened. Counts for file descriptors should be reset on “exec”. This information is returned in the user-supplied iostats structure. Returns 0 on success, or -1 on failure (e.g. bad file descriptor).

You will need to:

- Modify the structure for an open file to track # of bytes read and written.
- Modify the implementations of the open, read and write sytem calls to update these new fields.
- Add the new getiostats system call to read these fields.
- Add the “struct iostats” definition to stat.h

Hint: Look at how the other system calls that use a file descriptor work.

## Setting up for the tests

The three test programs (test1.c, test2.c, test3.c) are not built into the xv6 image by default in the starter code, since they rely on a system call that doesn’t exist yet.

Make sure to add them to the UPROGS list in the Makefile. This is nessisary to pass the autograding tests.

## Manual Testing

- Run xv6 with “make qemu-nox”
- Run run “test1”, “test2”, “test3”, or any additional test program you choose to write for debugging.
- Run “halt” to exit the xv6 session.

## Debugging

- Plan A: Use “cprintf” from kernel code to print debug messages.
- Plan B: Use the “make qemu-gdb” command to run the whole system in GDB.

## Learning objectives

The goal of this assignment is for you to explore the xv6 source code and find the appropriate things to modify. After this assignment you should be familiar with:

- The execution flow of a system call.
- Where data about open files is stored in the kernel data structures.
- How process-related data is stored is handled in the kernel in general.

Because the goal is to search around and find stuff, it would be best if you avoid posting exact function names or line numbers that need to be changed to complete this assignment on Piazza.

## To Submit:
- A tarball containing the starter code and your modifications.
- There should be a single top level directory containing the same structure as provided in the starter code.
- Make sure you don’t change the test script or top-level Makefile.
- No executables or object files.
- Make extra sure to run “make clean” before submitting.
- No extra hidden files.
