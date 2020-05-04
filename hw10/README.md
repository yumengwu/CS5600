## Description

## Concept: Sample Sort

Sample sort is a parallel version of quicksort.

It works as follows:

Input:

- An array of N items
- An integer P for number of threads to use.

Result:

- The input array has been sorted.

Steps:

- Sample

    - Randomly select 3*(P-1) items from the array.
    - Sort those items.
    - Take the median of each group of three in the sorted array, producing an array (samples) of (P-1) items.
    - Add 0 at the start and +inf at the end (or the min and max values of the type being sorted) of the samples array so it has (P+1) items numbered (0 to P).

- Partition

    - Spawn P threads, numbered p in (0 to P-1).
    - Each thread builds a local array of items to be sorted by scanning the full input and taking items between samples[p] and samples[p+1].
    - Write the number of items (n) taken to a shared array sizes at slot p.

- Sort locally

    - Each thread uses some sorting function to sort the local array.

- Copy local arrays to output.

    - Each thread calculates where to put its result array as follows:

        - start = sum(sizes[0 to p - 1]) # that’s sum(zero items) = 0 for p = 0
        - end = sum(sizes[0 to p]) # that’s sum(1 item) for p = 0

    - Warning: Data race if you don’t synchronize here.
    - Each thread copies its sorted array to output[start..end]

- Cleanup

    - Terminate the P worker threads.

## Task 0: Do Rustlings

Find Rustlings at https://github.com/rust-lang/rustlings

You don’t need to submit anything for this part of the assignment, but course staff may want to see your solutions to these problems if you’re having trouble with simple Rust concepts.

## Task 1: Sample Sort in Rust

- Complete the provided ssort program, written in Rust.
- This should be an implementation of sample sort, using threads (std::thread) for parallelism.
- Use the Rust standard library to avoid data races while maintaining a parallel speedup (e.g std::sync::Mutex, std::sync::Barrier).
- You should not write any “unsafe” rust code.
- Your program, psort, should take three arguments:

    - An integer specifying the number of threads to sort with
    - The input file of data to sort
    - An output file where the sorted data will be written

- The input file and output file shoudl be structured as follows:

    - First 8 bytes: long size - number of floats in the file
    - Next 4*size bytes: An array of floats

- Use std::fs::File for I/O
- Use sample sort to sort the data in memory
- Use the same basic flow as with HW06, including a shared “sizes” array.
- Handle errors by crashing as soon as possible.
- Your rust program should compile with no warnings.

In addition, each worker thread should print out some text, showing that it’s actually participating in sample sort:

 {p}: start {start}, count {length of local array}

Example session:

\$ ./tools/gen-input 20 data.dat<br/>
\$ ./ssort 4 data.dat output.dat<br/>
0: start 0.0000, count 3<br/>
1: start 7.5690, count 5<br/>
2: start 27.1280, count 11<br/>
3: start 95.5110, count 1<br/>
\$ ./tools/check-sorted output.dat<br/>
\$<br/>

## Task 2: Measure Parallel Speedup

Note that Rust 1.39.0 is installed on the login server. This is recent enough that you should be able to use it for these tests. Avoid any features that requre later Rust versions.

Test your Rust sample sort program in six scenarios, each with a newly generated input file of 50 million floats (or enough to take at least 10 seconds for 1 thread on your machine):

- Your local machine, 1 thread
- Your local machine, 4 threads
- Your local machine, 8 threads
- The CCIS server, 1 thread
- The CCIS server, 4 threads
- The CCIS server, 8 threads

*DO NOT* run your program on the CCIS server until you’ve completed the timing process locally and confirmed that your program runs in a reasonable amount of time.

Time these tests using the “/usr/bin/time -p” command. (You’re interested in “real” time)

Create a graph, graph.png, showing how your program scales with 1, 4, and 8 processes on the two test machines.

## Task 3: Write a Report

Create a file, report.txt, containing the following information:

- The operating system, processor model, number of processor cores, and amount of RAM for your local machine.
- The operating system, processor model, number of processor cores, and amount of RAM for the CCIS server.
- A table of measured time and parallel speedup for each test (use ASCII art).
    - 6 test scenarios with threads
- Parallel speedup is calculated as (time for 1 thread / time for N threads)
- Optimal parallel speedup is N.
- Two or three paragraphs discussing what results you got and why you got them.
- Two or three paragraphs discussing the pros and cons of Rust as an alternative to C for this sort of program.

## To Submit

- A .tar.gz archive.
- The archive should follow the directory structure of the starter code, including a single top-level directory and separate subdirectories for tools and ssort.
- The contents of the tools directory should be submitted unmodified.
- A report.txt (which should be plain Unix text, line wrapped to 70 columns)
- One graph: graph.png which should be a PNG images
- No binaries, object files, extra hidden files, or input/output data files.
- Double check that you have typed “make clean” and aren’t submitting object files, binaries, or data files.
