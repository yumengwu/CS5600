# CS 5600 HW01: Linux Dev Environment

## Part 1: Install a Linux VM

This class covers use of the APIs provided by the operating system to running programs. OS APIs and behavior differ, so we’re going to look at a concrete example: Debian 10

In order to be able to develop and test on this platform, you should have it installed locally.

In addition, Khoury College provides a shared Linux server for student use that you should be familiar with.

### Task 1:
- Install Debian 10 (64-bit) in a VirtualBox VM on your own computer.
    - See the [guide here](http://ccs.neu.edu/home/ntuck/guides/2020/debian-vm.html) for more details
    - Make sure to allocate enough RAM and disk space, and to install development tools as described in the guide.
- Use the “apt” tool to install “screenfetch” program in your VM.
- Run the program in a terminal window in your VM and take a screenshot of your full VM window including the VirtualBox menus and border showing the terminal output and Debian desktop.

Some notes on the VM:

- You’re not required to use this VM for future assignments, but doing so is recommended. A full Linux system is necessary to complete some assignments.
- If you decide to share a directory between your VM and host OS, you should not work in that directory. Several assignments will not function correctly in a shared directory and need to be run on a normal local Linux filesystem.
- This task is required even if you’re running Linux natively on your computer. I’ll admit the requirement is silly if you’re already running Debian 10, but that just makes it both silly and required.

### Task 2:
- Get a Khoury Linux account if you don’t already have one.
    - Request page should be http://my.ccs.neu.edu/
- Figure out how to SSH into the server.
    - Do this from your Linux VM with “ssh yourname@login.ccs.neu.edu”.
    - Do this from your main OS. On Linux/Mac it’s the same as above, for Windows use a search engine like Bing to search for “putty ssh”.
- Run the command “less /course/cs3650/help/unix-help.txt” and read the help file. The “q” key quits.
- Run “mkdir -p ~/cs3650/hw01”.
- Run “ps -ef” and redirect the output to a file named “ps-output.txt” in your new hw01 directory.
- From your Linux VM, use the “scp” command to copy the “ps-output.txt” file from the login server to your local VM.

## Part 2: C and Assembly

C and Assembly programs are structured the same way: as a collection of functions, possibly spread across multiple source files.

But C and Assembly are even more closely related: they both have the same concept of a “function”. A function written in either language can be called transparently from the other.

In this assignment, we’ll confirm that you have a development environment that allows you to write, build, and run programs written in C and Assembly.

### A note on tools

It is recommended that you work using your Debian VM from Part 1 with the vim editor. (“sudo apt install vim-nox”; Read this StackOverflow thread: [vim tab=4 spaces](https://stackoverflow.com/questions/234564/tab-key-4-spaces-and-auto-indent-after-curly-braces-in-vim)).

Using the Debian VM is optional, but strongly recommended. Several assignments will not work in alternate setups like Mac OS or WSL. If your host OS is a modern Linux distribution everything should work fine there.

Using the vim editor is optional, but recommended. If you insist on using (or starting with) an editor where Ctrl+C is copy, I recommend Kate (“sudo apt install kate”). Try to avoid very simple editors like nano/mousepad or IDEs like Eclipse - those are not the right tools for this class.

### Using homework tarballs
The starter code for this assignment is distributed as a .tar.gz archive (a “gzipped tarball”).
- You can unpack this tarball with: <font color="#e83e8c">tar xzvf [file].tar.gz</font>
- he starter code is unpacked into a directory. You want to keep this directory and nested directory structure so as to not confuse the autograding scripts.
- Once you’ve completed your work in the assignment directory, you can pack it up into a new tarball for submission with: <font color="#e83e8c">tar czvf [new-file].tar.gz [the-directory]</font>

Example:

\$ tar xzvf hw01-starter.tar.gz
... tarball is unpacked
\$ cd hw01-starter
\$ vim foo.c
... modify the starter code
\$ cd ..
\$ tar czvf hw01-yourname.tar.gz hw01-starter
... new tarball is created

### Task 1:
- Create a C source file, “square.c”, containing a function called “square” that squares a long integer.
- Run “make square” and “./square 5” to verify that the provided assembly code in “square-main.s” can call your function.

### Task 2:
- Create an amd64 assembly source file, “cube.s”, containing a function called “cube” that cubes a long integer (e.g. cube(3) = 27).
- You are expected to write the assembly code. Compiler output won’t be accepted.
- Run “make cube” and “./cube 5” to verify that the provided C code in “cube-main.c” can call your function.

### Task 3:
- To prepare for i386 development on amd64, run “sudo apt install gcc-multilib”
- Create an i386 assembly source file, “add1.s”, containing a function called “add1” that adds 1 to a long integer (e.g. add1(5) = 6).
- You are expected to write the assembly code. Compiler output won’t be accepted.
- Run “make add1” and “./add1 5” to verify that the provided C code in “add1-main.c” can call your function.

### Task 4:
- Run “perl test.pl” to make sure that the autograder will be happy with your code. Fix any issues.

## To Submit:
- A tarball containing a single top-level directory with:
    - the starter code and your modifications.
    - Your screenshot from Part 1, named “screenshot.jpg”
    - Your process list from Part 1, named “ps-output.txt”
- Make sure you don’t change the test script or Makefile.
- No extra hidden files.

Hint: Check your submission on Inkfish to make sure you’ve submitted an archive with the correct contents and structure.

Hint: Inkfish will automatically run the test script and show the results. Failed tests mean lost points. You can resubmit multiple times to fix failed tests, so make sure you check the test output after submitting.
