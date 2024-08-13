### How to decompile with multiple tools

In order to understand the code for level8, we clearly needed multiple decompiler since only ghidra did not make things clear enough for us to translate the code into a logic and easy c program.

So for this purpose we use dogbolt which is an online decompiler that makes things way easier even if it has way less details than the full ghidra version. You can see the result here: (dogbolt.png)

![DogBolt](https://github.com/kbarbry/RainFall/blob/main/level8/Resources/dogbolt.png)

For this exercice we used angr, BinaryNinja, Hex-Rays and Reko that seemed to have the clearest version of this binary, since we already use ghidra we don't need to re-use it in dogbolt.

### What is the full log of the core dump

When defining auth, then freeing it 2 times in a raw, the server crashes and we get a core dump, this is the full log:

```sh
level8@RainFall:~$ ./level8 
(nil), (nil) 
auth 
0x804a008, (nil) 
reset
0x804a008, (nil) 
reset
*** glibc detected *** ./level8: double free or corruption (fasttop): 0x0804a008 ***
======= Backtrace: =========
/lib/i386-linux-gnu/libc.so.6(+0x74f82)[0xb7ea0f82]
./level8[0x8048678]
/lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0xb7e454d3]
./level8[0x80484d1]
======= Memory map: ========
08048000-08049000 r-xp 00000000 00:10 12541      /home/user/level8/level8
08049000-0804a000 rwxp 00000000 00:10 12541      /home/user/level8/level8
0804a000-0806b000 rwxp 00000000 00:00 0          [heap]
b7e07000-b7e23000 r-xp 00000000 07:00 17889      /lib/i386-linux-gnu/libgcc_s.so.1
b7e23000-b7e24000 r-xp 0001b000 07:00 17889      /lib/i386-linux-gnu/libgcc_s.so.1
b7e24000-b7e25000 rwxp 0001c000 07:00 17889      /lib/i386-linux-gnu/libgcc_s.so.1
b7e2b000-b7e2c000 rwxp 00000000 00:00 0 
b7e2c000-b7fcf000 r-xp 00000000 07:00 17904      /lib/i386-linux-gnu/libc-2.15.so
b7fcf000-b7fd1000 r-xp 001a3000 07:00 17904      /lib/i386-linux-gnu/libc-2.15.so
b7fd1000-b7fd2000 rwxp 001a5000 07:00 17904      /lib/i386-linux-gnu/libc-2.15.so
b7fd2000-b7fd5000 rwxp 00000000 00:00 0 
b7fd8000-b7fdd000 rwxp 00000000 00:00 0 
b7fdd000-b7fde000 r-xp 00000000 00:00 0          [vdso]
b7fde000-b7ffe000 r-xp 00000000 07:00 17933      /lib/i386-linux-gnu/ld-2.15.so
b7ffe000-b7fff000 r-xp 0001f000 07:00 17933      /lib/i386-linux-gnu/ld-2.15.so
b7fff000-b8000000 rwxp 00020000 07:00 17933      /lib/i386-linux-gnu/ld-2.15.so
bffdf000-c0000000 rwxp 00000000 00:00 0          [stack]
Aborted (core dumped)
```
