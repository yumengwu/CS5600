## Description

n this assignment you’ll be adding functionality to a simple FUSE filesystem.

Keep in mind that the TAs will be manually testing your submission for requested functionality that isn’t fully covered by the automated tests.

## Part 1: Standard Functonality

Extend the filesystem provided as starter code to support more functionality:

- Read and write from files larger than one block. For example, you should be able to support one hundred 1k files or five 100k files.
- Create directories and nested directories. Directory depth should only be limited by disk space (and possibly the POSIX API).
- Remove directories.
- Hard links.
- Symlinks
- Support metadata (permissions and timestamps) for files and directories.
    - Don’t worry about multiple users. Assume the user mounting the filesystem is also the owner of any filesystem objects.
- Anything else that’s covered in the tests.

## Part 2: Copy on Write

Modify your file system to use copy on write for all operations that modify the filesystem. Each of these operations should create a new version of the filesystem, with a new root directory that has shared structure with previous versions of the filesystem.

The operations that modify the filesystem and should therefore create a new version are:

- write
- truncate
- mknod
- unlink
- link
- rename
- chmod
- set time

Your filesystem should preserve at least the 7 most recent previous versions.

In order to be able to see this copy-on-write versioning functionality, extend the functionality of the filesystem tool to include two new operations:

- List versions
- Rollback to a previous version

Examples:

\$ ./cowtool versions disk0.cow<br/>
Versions for disk0.cow:<br/>
16 unlink /x3.txt<br/>
15 write /x5.txt<br/>
14 truncate /x5.txt<br/>
13 mknod /x5.txt<br/>
12 write /x4.txt<br/>
11 truncate /x4.txt<br/>
10 mknod /x4.txt<br/>
9 write /x3.txt<br/>
\$ ./cowtool rollback disk0.cow 15<br/><br/>
Rollback disk0.cow to version 15<br/>
\$ ./cowtool ls disk0.cow<br/>
...<br/>
/x1.txt<br/>
/x2.txt<br/>
/x3.txt<br/>
/x4.txt<br/>
/x5.txt<br/>
In your list of versions, the first version in the list must be the current version, and that must be the second line of output.

Your version numbers must be assigned in consecutive order, and you can assume that a 32-bit int won’t overflow.

## To Submit

- A .tar.gz archive
- With the directory layout from the starter code
- containing source code
- no stray files.
