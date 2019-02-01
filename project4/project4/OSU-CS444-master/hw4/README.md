# cs444-015 Project 2: I/O Elevators

Due: 23:59:59, 24 October 2016
===========
In this project you will be developing your solution on the virtual machine.
Using the yocto Linux kernel sources as described in HW1, you are asked to
implement the following:
* Implement the LOOK I/O scheduler

Develop your solution based off the currect FIFO (noop) implementation currently
in the block directory. You will be required to write a new file titled
sstf-iosched.c.
To receive full credit, your solution must be correct and working on the virtual
machine provided.

Some points to keep in mind:
* This is an elevator algorithm
* Choose either both directions of travel or only servicing in one direction (LOOK or C-LOOK)
* There will be at least 2 sorts involved.
* Don't forget to MERGE. Either front or back, not both.
* You will need to disable virtio on qemu in order to do this assignment. I leave
it to you to figure out how to do that. Please see the qemu man pages for
details if you're lost.
* I really hope you actually did look up the different options for qemu, as you'll
need to make a change this time to get your IO scheduler to be used.

You are also required to do the following write-up including:
* The design you plan to use to implement the SSTF algorithms.
* Your version control log.
* A work log, detailing what you did when. Ideally, this is the same as the above.
* Answer the following questions in sufficient detail:
  1. What do you think the main point of this assignment is?
  2. How did you personally approach the problem? Design decisions, algorithm, etc.
  3. How did you ensure your solution was correct? Testing details, for instance.
  4. What did you learn?
  
Make sure your submission is a tarball labelled CS444_project2_#.tar.bz2 and
includes the following:
* A linux patch file containing your solution
* The write-up in LaTeX format.
* A makefile for the LaTeX file.
