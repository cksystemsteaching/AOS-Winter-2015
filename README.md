Home Work Flow
==============

* Step 0: form a team of 2-3 members
* Step 1: get a github account (for each member)
* Step 2: one person per group forks the AOS-Winter-2015 repository by clicking [here](https://github.com/cksystemsteaching/AOS-Winter-2015/fork) and adds the other team members as collaborators
* Step 3: check out the branch named __selfie-master__ in __your__ fork of AOS-Winter-2015
* Step 4: implement the first assignment (see below)
* Step 5: add your names to the AUTHORS file
* Step 6: send a pull request containing your solution via github.com to [cksystemsteaching/AOS-Winter-2015/tree/selfie-master](https://github.com/cksystemsteaching/AOS-Winter-2015/tree/selfie-master)


Assignment 0: Basic data structures
-----------------------------------

Review [linked lists](https://en.wikipedia.org/wiki/Linked_list) and implement a simple program using a singly linked list in C*. The minimal requirements are as follows:

* must be implemented in C*
* must compile with selfie
* must run on selfie
* the list must be dynamically allocated
* every node must be dynamically allocated
* inserting nodes to the list and removing nodes from the list
* list iteration
* Bonus: sort the list. Any way you like
* Deadline: Oct 15, end of day


Assignment 1: Loading, scheduling, switching, execution
-------------------------------------------------------

Implement basic concurrent execution of _n_ processes in mipster. _n >= 2_ 

* understand how mipster [interprets and executes binary instructions](https://github.com/cksystemsteaching/AOS-Winter-2015/blob/selfie-master/selfie.c#L3933). Tipp: add your own comments to the code
* mipster maintains a local state for a process (running executable), e.g., pc, registers, memory
* understand the purpose of each variable and data structure
* duplicate the process state n times
* running mipster like: _./selfie -m 32 yourbinary_ should generate _n_ instances of _yourbinary_ in a single instance of mipster
* implement [preemptive multitasking](https://en.wikipedia.org/wiki/Preemption_(computing)), i.e., switching between the _n_ instances of _yourbinary_ is determined by mipster 
* switch processes every m instructions. _1 <= m <= number of instructions in yourbinary_
* implement [round-robin scheduling](https://en.wikipedia.org/wiki/Round-robin_scheduling)
* add some output in _yourbinary_ to demonstrate context switching
* Deadline: Oct 22, end of day