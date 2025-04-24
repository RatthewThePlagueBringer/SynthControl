# SynthControl
Lightweight Linux kernel-level implementation of Open Sound Control (OSC).

To use our system calls, find the section of each file with our new code (for syscall_64.tbl it is located from ID 435-447, all others are at the end of the files) and add it to their respective locations in your kernel. Refer below for the file locations within the kernel:
- syscall_64.tbl - located at arch/x86/entry/syscalls/syscall_64.tbl
- syscalls.h - located at include/linux/syscalls.h
- sys.c - located at kernel/sys.c

