# os - operating system

An x86-64 hobby operating system written in C and assembly. It features a monolithic kernel and a userspace and libc.

## Building

A simple `make` should build all dependencies and an iso file.
Run the iso in qemu with `make run`, this target also builds the iso as a dependency.
Run `make install dev=...` to install the iso file to the specified device.

## Userspace Development

The userspace applications use a kbuild style build system.
Make a userspace application with `make -C ... M=$(pwd)` from the userspace application toplevel directory where the path after -C points at the `user` directory.
All userspace applications should live in the `user` directory.
In the toplevel of the applications directory there should be a Makefile that defines the following variables:
- `TARGET`: the name of the resulting binary
- `CFILES`: the files that belong to the application
Optionally the following variables can be defined:
- `ASMFILES`: the assembly files that belong to the application
- `CFLAGS`: flags that are passed to gcc in addition to the default flags
- `LDFLAGS`: flags that are passed to ld in addition to the default flags

Relative paths are allowed in `CFILES` and `ASMFILES` variables.
They are substituted by their absolute path equivalents.
Paths in the `CFLAGS` and `LDFLAGS` should be absolute, the `M_PWD` variable holds the absolute path of the toplevel directory of the userspace application.
