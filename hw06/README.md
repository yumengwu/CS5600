# Description

# Concept: Sample Sort

Sample sort is a parallel version of quicksort.

It works as follows:

Input:

- An array of N items
- An integer P for number of processes to use.

Result:

- The input array has been sorted.

Steps:

- Sample
    - Randomly select 3*(P-1) items from the array.
    - Sort those items.
    - Take the median of each group of three in the sorted array, producing an array (samples) of (P-1) items.
    - Add 0 at the start and +inf at the end (or the min and max values of the type being sorted) of the samples array so it has (P+1) items numbered (0 to P).

- Partition
    - Spawn P processes, numbered p in (0 to P-1).
    - Each process builds a local array of items to be sorted by scanning the full input and taking items between samples[p] and samples[p+1].
    - Write the number of items (n) taken to a shared array sizes at slot p.

- Sort locally
    - Each process uses quicksort to sort the local array.

- Copy local arrays to input.
    - Each process calculates where to put its result array as follows:
        - start = sum(sizes[0 to p - 1]) # that’s sum(zero items) = 0 for p = 0
        - end = sum(sizes[0 to p]) # that’s sum(1 item) for p = 0
    - Warning: Data race if you don’t synchronize here.
    - Each process copies its sorted array to input[start..end]

- Cleanup
    - Terminate the P subprocesses. Array has been sorted “in place”.

# Homework Tasks

## Task 1: Sample Sort with Processes

### Task 1.1: Complete the ‘psort’ program.

- Complete the provided ‘psort’ program.
- This is an implementation of sample sort, using processes (with fork) for parallelism.
- Use the provided barrier primitive to avoid data races.
- The source for this program should be in a directory called psort.
- Your program, psort, should take two arguments:
    - An integer specifying the number of processes to sort with
    - The file to sort

- The input file is structured as follows:
    - First 8 bytes: long size - number of floats in the file
    - Next 4*size bytes: An array of floats

- Use mmap for I/O on the file to be sorted.
- Use mmap to share data between processes.
    - Specifically, you should have a shared array of partition

- Use sample sort to sort the input file, in place.

In addition, each subprocess should print out some text, showing that it’s actually participating in sample sort:

{p}: start {start}, count {length of local array}

Example session:

\$ ./tools/gen-input 20 data.dat
\$ ./psort 4 data.dat
0: start 0.0000, count 3
1: start 7.5690, count 5
2: start 27.1280, count 11
3: start 95.5110, count 1
\$ ./tools/check-sorted data.dat
\$

### Task 1.2: Measure Parallel Speedup

Test your process-based sample sort program in six scenarios, each with a newly generated input file of 50 million floats (or enough to take at least 10 seconds for 1 process on your machine):

- Your local machine, 1 process
- Your local machine, 4 processes
- Your local machine, 8 processes
- The CCIS server, 1 process
- The CCIS server, 4 processes
- The CCIS server, 8 processes

*DO NOT* run your program on the CCIS server until you’ve completed the timing process locally and confirmed that your program runs in a reasonable amount of time and terminates all its child processes.

Time these tests using the “/usr/bin/time -p” command. (You’re interested in “real” time)

Create a graph, graph1.png, showing how your program scales with 1, 4, and 8 processes on the two test machines.

## Task 2: Sample Sort with Threads

### Task 2.1: Complete the ‘tsort’ program.

Complete the second version of the sorting program. It should be just like the “psort” program except:

- It should be named “tsort” and have its source code in a “tsort” directory.
- It should use threads (e.g. pthread_create) instead of processes.
- It should not use mmap to allocate the shared sizes array, instead just using memory allocated normally and shared between threads.
- You’ll need to implement your own barrier using mutexes and condition variables (no semaphores).

### Task 2.2: Benchmark Your Second Program

Step 1: Pick a machine to test on.

- If your local machine can run 8 parallel threads with a speedup (i.e. you have an 8+ core processor), test there.
- If not, test on the CCIS server.

Step 2: Verify that your program isn’t broken.

- Run on your local machine with 8 threads and verify that your program works and terminates.

Step 3: Benchmark

*DO NOT* run your program on the CCIS server until you’ve tested the timing process locally and confirmed that your program runs in a reasonable amount of time and terminates normally.

Test your thread-based sample sort program in three scenarios, each with a newly generated input file of 50 million floats (or enough to take at least 10 seconds for 1 process on your machine):

- Your test machine, 1 process
- Your test machine, 4 processes
- Your test machine, 8 processes

Create a graph, graph2.png, comparing scaling for threads vs. processes on the machine where you tested both.

## Task 3: Write a Report

Create a file, report.txt, containing the following information:

- The operating system, processor model, number of processor cores, and amount of RAM for your local machine.
- The operating system, processor model, number of processor cores, and amount of RAM for the CCIS server.
- A table of measured time and parallel speedup for each test (use ASCII art).
    - 6 test scenarios with processes
    - 3 more test scenarios with threads

- Parallel speedup is calculated as (time for 1 process / time for N processes)
- Optimal parallel speedup is N.
- Two or three paragraphs discussing what results you got and why you got them.
- Look up “Amdahl’s Law” and “Gustafson’s Law” on Wikipedia. Is sample sort a good parallel sorting algorithm?
- Which performs better, threads or processes? By how much? Why?

Once everything else is done, do one more test and add one final section (around one paragraph) to your report:

- Modify one of your programs to sample more than 3*(P-1) items (e.g. maybe 61*(P-1) instead) and then compare 1 vs. 8 processes/threads.
- Does this improve your parallel speedup?
- Why do you get the results you see? Is doing more sequential work a good deal?

## To Submit

- A .tar.gz archive.
- The archive should follow the directory structure of the starter code, including a single top-level directory and separate subdirectories for tools, psort, and tsort.
- The contents of the tools directory should be submitted unmodified.
- A report.txt (which should be plain Unix text, line wrapped to 70 columns)
- Two graphs: graph1.png and graph2.png, both of which should be PNG images
- No binaries, object files, extra hidden files, or input/output data files.
