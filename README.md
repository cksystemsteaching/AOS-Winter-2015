# Selfie

Selfie is a project of the [Computational Systems Group](http://www.cs.uni-salzburg.at/~ck) at the Department of Computer Sciences of the University of Salzburg in Austria.

For further information and support please refer to http://selfie.cs.uni-salzburg.at

## Build Instructions

### On 32-bit Linux

The first step is to produce a binary that runs on your computer. To do that use `gcc` in a terminal to compile `selfie.c`:

```bash
$ gcc selfie.c -o selfie
```

This produces from `selfie.c` an executable called `selfie` as directed by the `-o` option. The executable contains both the C\* compiler as well as the mipster emulator.

Selfie may be invoked as follows:

```bash
./selfie { -c source | -o binary | -l binary } [ -m size ... | -k size ... ]
```

The order in which the options are provided matters for taking full advantage of self-referentiality. The `-c` option invokes the C\* compiler on the given `source` file producing MIPSter code that is stored internally. The `-o` option writes MIPSter code produced by the most recent compiler invocation to the given `binary` file. The `-l` option loads MIPSter code from the given `binary` file. The `-m` option invokes the mipster emulator to execute MIPSter code most recently loaded or produced by a compiler invocation. The emulator creates a machine instance with `size` MB of memory. The `source` or `binary` name of the MIPSter code and any remaining `...` arguments are passed to the main function of the code. The `-k` option is not yet supported.

To compile `selfie.c` for mipster use the following commands:

```bash
$ gcc selfie.c -o selfie
$ ./selfie -c selfie.c -o selfie.mips
```

This produces a MIPSter binary file called `selfie.mips` that implements selfie.

To execute `selfie.mips` by mipster use the following command:

```bash
$ ./selfie -l selfie.mips -m 32
```

This is semantically equivalent to executing `selfie` without any arguments:

```bash
$ ./selfie
```

### Self-compilation

Here is an example of how to perform self-compilation of `selfie.c`:

```bash
$ gcc selfie.c -o selfie
$ ./selfie -c selfie.c -o selfie1.mips -m 32 -c selfie.c -o selfie2.mips
$ diff -s selfie1.mips selfie2.mips
Files selfie1.mips and selfie2.mips are identical
```

### Self-execution

The following example shows how to perform self-execution of `selfie.c`. In this case we invoke the emulator to invoke itself which then invokes the compiler to compile itself:

```bash
$ gcc selfie.c -o selfie
$ ./selfie -c selfie.c -o selfie1.mips -m 64 -l selfie1.mips -m 32 -c selfie.c -o selfie2.mips
$ diff -s selfie1.mips selfie2.mips
Files selfie1.mips and selfie2.mips are identical
```

Note that the example may take several hours to complete. Also, a machine instance A running a machine instance B needs more memory than B, say, 64MB rather than 32MB in the example here.

### Work Flow

To compile any C\* source you may use `selfie` directly or on top of the emulator. Both generate identical MIPSter binaries:

```bash
$ gcc selfie.c -o selfie
$ ./selfie -c any-cstar-file.c -o any-cstar-file1.mips
$ ./selfie -c selfie.c -o selfie.mips
$ ./selfie -l selfie.mips -m 32 -c any-cstar-file.c -o any-cstar-file2.mips
$ diff -s any-cstar-file1.mips any-cstar-file2.mips
Files any-cstar-file1.mips and any-cstar-file2.mips are identical
```

The same can also be done without producing a `selfie.mips` binary file:

```bash
$ gcc selfie.c -o selfie
$ ./selfie -c any-cstar-file.c -o any-cstar-file1.mips
$ ./selfie -c selfie.c -m 32 -c any-cstar-file.c -o any-cstar-file2.mips
$ diff -s any-cstar-file1.mips any-cstar-file2.mips
Files any-cstar-file1.mips and any-cstar-file2.mips are identical
```

And even with a single invocation of `selfie`:

```bash
$ gcc selfie.c -o selfie
$ ./selfie -c any-cstar-file.c -o any-cstar-file1.mips -c selfie.c -m 32 -c any-cstar-file.c -o any-cstar-file2.mips
$ diff -s any-cstar-file1.mips any-cstar-file2.mips
Files any-cstar-file1.mips and any-cstar-file2.mips are identical
```

#### Debugging

Console messages always begin with the name of the source or binary file currently running. The emulator also shows the amount of memory allocated for its machine instance and how execution terminated (exit code).

You can enable verbose debugging with variables set in `selfie.c`:

 - debug_diassemble: Print disassemble output on the screen
 - debug_registers: Print register content
 - debug_syscalls: Print debugging information on every system call
 - debug_load: Print hex code of what the emulator loaded

### On Mac OS X / 64-bit Linux

On Mac OS X as well as on 64-bit Linux (requires gcc-multiarch or, on Ubuntu, gcc-multilib), you may use the following command to compile `selfie.c`:

```bash
clang -w -m32 -D'main(a, b)=main(int argc, char **argv)' selfie.c -o selfie
```

After that, you can proceed with the same commands as for 32-bit Linux.

The `-w` option suppresses warnings that can be ignored for now. The `-m32` option makes the compiler generate a 32-bit executable. Selfie only supports 32-bit architectures right now. The `-D` option is needed to bootstrap the main function declaration. The `char` data type is not available in C\* but required by `clang`.