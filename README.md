<<<<<<< HEAD
# Selfie

Selfie is a project of the [Computational Systems Group](http://www.cs.uni-salzburg.at/~ck) at the Department of Computer Sciences of the University of Salzburg in Austria.

For further information and support please refer to http://selfie.cs.uni-salzburg.at

## Build Instructions

### On 32-bit Linux

The first step is to produce a binary that runs on your computer. To do that, use `gcc` in a terminal to compile `selfie.c`:

```bash
$ gcc -o selfie selfie.c
```

This produces an executable called `selfie` as directed by the `-o` option. The executable contains both the C\* compiler as well as the MIPSter emulator.

Selfie may be invoked as follows:

```bash
./selfie [ -c | -m <memory size in MB> <binary> ]
```

When using `selfie` with the `-c` option or without any arguments the compiler is invoked. With the `-m` option the emulator is invoked and configured to create a machine instance with \<memory size in MB\> that loads and executes \<binary\>.

To compile `selfie.c` for MIPSter, use the following commands. Be aware that the compiler requires an empty file called `out` in the current execution directory to write its output to it.

```bash
$ gcc -o selfie selfie.c
$ touch out
$ ./selfie < selfie.c
$ mv out selfie.mips
```

### Self-compilation

Here is an example of how to test self-compilation of `selfie.c`:

```bash
$ gcc -o selfie selfie.c
$ touch out
$ ./selfie -c < selfie.c
$ mv out selfie.mips1
$ touch out
$ ./selfie -m 32 selfie.mips1 < selfie.c
$ mv out selfie.mips2
$ diff -s selfie.mips1 selfie.mips2
Files selfie.mips1 and selfie.mips2 are identical
```

### Self-execution

The following example shows how to test self-execution of `selfie.c`. In this case we invoke the emulator to invoke itself which then invokes the compiler to compile itself:

```bash
$ gcc -o selfie selfie.c
$ touch out
$ ./selfie < selfie.c
$ mv out selfie.mips1
$ touch out
$ ./selfie -m 64 selfie.mips1 -m 32 selfie.mips1 < selfie.c
$ mv out selfie.mips2
$ diff -s selfie.mips1 selfie.mips2
Files selfie.mips1 and selfie.mips2 are identical
```

### Work Flow

To compile any C\* source you may use `selfie` directly or `selfie.mips` on top of the emulator. Both generate identical MIPSter binaries:

```bash
$ gcc -o selfie selfie.c
$ touch out
$ ./selfie < selfie.c
$ mv out selfie.mips
$ touch out
$ ./selfie < any-cstar-file.c
$ mv out any-cstar-file.mips1
$ touch out
$ ./selfie -m 32 selfie.mips < any-cstar-file.c
$ mv out any-cstar-file.mips2
$ diff -s any-cstar-file.mips1 any-cstar-file.mips2
Files any-cstar-file.mips1 and any-cstar-file.mips2 are identical
```

#### Debugging

By default, the boot prompt shows the amount of memory used by the emulator instance and how execution of the binary file terminated (exit code).

You can enable verbose debugging with variables set in `selfie.c`:

 - debug_diassemble: Print disassemble output on the screen
 - debug_registers: Print register content
 - debug_syscalls: Print debugging information on every system call
 - debug_load: Print hex code of what the emulator loaded

### On Mac OS X / 64-bit Linux

On Mac OS X as well as on 64-bit Linux (requires gcc-multiarch. On Ubuntu, install gcc-multilib), you may use the following command to compile `selfie.c`:

```bash
clang -w -m32 -D'main(a, b)=main(int argc, char **argv)' -o selfie selfie.c
```

After that, you can proceed with the same commands as for 32-bit Linux.

The `-w` option suppresses warnings that can be ignored for now. The `-m32` option makes the compiler generate a 32-bit executable. Selfie only supports 32-bit architectures right now. The `-D` option is needed to bootstrap the main function declaration. The `char` data type is not available in C\* but required by `clang`.
=======
Home Work Flow
--------------

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
>>>>>>> d59eedb453fca7fa76fefc25599d40eb13730f77
