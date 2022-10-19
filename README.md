# os - operating system

An x86_64 hobby operating system written in C and assembly. It features a monolithic kernel and a dedicated scripting language that is used for all userspace applications.

## Building

Follow this tutorial to build the [cross compiler](https://wiki.osdev.org/GCC_Cross-Compiler)
and [libgcc without red zone](https://wiki.osdev.org/Libgcc_without_red_zone).
Use `./cross_compiler` as the PREFIX, target is `x86_64-elf`.

Dependencies: `apt install nasm xorriso`.

