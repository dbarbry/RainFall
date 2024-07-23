### How to find m address ?

There is actually three ways, or you can check on ghidra and by searching well or double click on the m inside ghidra, you can arrive to this line:

```sh
m       XREF[2]:     Entry Point(*), v:080484da(R)  
0804988c 00 00 00 00     undefined4 00000000h
```

Or we can also analyze it via gdb by using the command p for history (yes)

```
(gdb) p &m
$1 = (<data variable, no debug info> *) 0x804988c
```

There is no history to the variable yet but gives us its declaration address.

Finally you can use objdump which will print you the entire list of symbols of the executable:

```sh
level3@RainFall:~$ objdump -t ./level3

./level3:     file format elf32-i386

SYMBOL TABLE:
08048134 l    d  .interp	00000000              .interp
[...]
0804988c g     O .bss	00000004              m
[...]
```

### More explanation on the buffer

This is how the final buffer looks like:

'\x8c\x98\x04\x08' + '%x ' * 3 + 'A' * 38 + '%n'

Let's break down more precisely every part of it:

- The address at the beginning is the address of m and is in little endian format for it to be interpreted as an address and not just a string, and also to work on the architecture we are using for RainFall.
- Then we print 3 times %x which moves the stack pointer 3 positions away, where the buffer is stored, which means that now the stack pointer will be precisely on the address of m.
- We add then 38 A (could be any values), why 38 tho ? If we print with check how the stack looks like by using "%x" input, this is what we see:
```sh
level3@RainFall:~$ ./level3 
%x %x %x %x %x 
200 b7fd1ac0 b7ff37d0 25207825 78252078
```
The first value of the stack we can see is the buffer size in hexadecimal. Then 2 addresses on the stack that we don't really care for, probably a saved pointer of EBP and maybe a return address. Then finally we have our 252078 repeated multiple time, which is the hexadecimal values of "%x ". The number of chars printed so far is 22 counting the spaces (it is part of chars printed by printf). We know that we need 64 chars, so quick maths gives... 42 ? Let's not forget that even if not printed as an address because it is interpreted by printf, the address counts as 4 chars, so substracted from 42, we get our 38 filling characters allowing us to reach a buffer of precisely 64 character.
- Finally we have the %n that makes everything work. At this state printf is searching for an address where to store the size of the buffer, the stack pointer is on the address of m. printf finds a buffer of size 64, stores it inside of m, the condition is fulfilled and we now have a shell as level4. 