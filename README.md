# SynthControl
Lightweight Linux kernel-level implementation of Open Sound Control (OSC).

## Project Description
Open Sound Control (OSC) is a lightweight audio message protocol used for sending audo messages to devices. One of its greatest points of pain is how there are hundreds of different versions and it can become unclear which version to use for your project. Our goal with this project was to create a standardized version of OSC for Linux that existed within the kernel itself, which not only streamlines the process of finding the correct version of OSC but also provides benefits such as message security and processing speeds by removing unecessary layers between device communication, since the kernel already handles that directly anyways.

## Installation Instructions

To use our system calls, find the section of each file with our new code (for syscall_64.tbl it is located from IDs 435-447, all others are at the end of the files) and add it to their respective locations in your kernel. Refer below for the file locations within the kernel:
- syscall_64.tbl - located at arch/x86/entry/syscalls/syscall_64.tbl
- syscalls.h - located at include/linux/syscalls.h
- sys.c - located at kernel/sys.c

After updating and saving the files, simply run the following commands:
```
sudo make
sudo make install
sudo make modules_install
```
Then reboot the device and you should be good to go!

## Syscall Documentation
Refer to the following chart for information regarding each system call:
| Syscall ID | Syscall Name | Description | Notes |
| :---: | --- | --- | --- |
| 435 | print_msg | prints the raw data for a message | Prints the information to the kernel log, use ```dmesg``` to see | 
| 436 | append_int | adds an integer argument to the message | | 
| 437 | append_int64 | adds a long long argument to the message | | 
| 438 | append_string | adds a character array argument to the message | | 
| 439 | append_char | adds a character argument to the message | | 
| 440 | append_color | adds a 4 RGBA values as an argument to the message | | 
| 441 | append_bool | adds a bool argument to the message | True and False are represented by 'T' and 'F' | 
| 442 | append_nil | adds a null flag argument to the message | Represented by 'N' | 
| 443 | append_infinitum | adds an infinity flag argument to the message | Represented by 'I' | 
| 444 | append_array_start | adds a start bracket to the message | | 
| 445 | append_array_end | adds an end bracket to the message | | 
| 446 | add_meta | populates the first 5 bytes of the message buffer with meta data regarding the number of arguments and their offsets | | 
| 447 | send_msg | sends the message through a UDP datagram to a listening OSC device | Currently does not function and only prints out the message, still under development | 

## Other Files
- main.c - the prototype C code we wrote for our OSC implementation before translating it to kernal-space C code.
- test.c - a testing file for simulating a program that sends OSC messages to an audio device. Run alongside client.c.
- client.c - a program that simulates an OSC device receiving OSC messages. Run alongside test.c.

## Acknowledgements
This project was developed as a senior design project at the University of Florida.

Special thanks to Dr. Alexandre Gomes de Siqueira, who was instrumental in advising our project development, as well as [tinyosc](https://github.com/mhroth/tinyosc?tab=readme-ov-file), a lightweight C implementation of OSC which served as a reference for our own code and helped us better understand OSC.
