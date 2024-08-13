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
            if (auth[8] == 0)
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
reset
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
```

## Important doc

[Multi decompiler](https://dogbolt.org/)