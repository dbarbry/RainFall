# bonus3

### First analysis

Here we are, to the last level. And we already have something noticeable when login in:

```sh
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   /home/user/bonus3/bonus3
```

We can onserve that here NX is enabled, whoch means some parts of the memory are not executable, it is up to the OS to decide which part of the memory allow code execution or not, typically the stack could block code execution, in this case the env variable containing a shell code would not work. Let's now analyze the code:

```c
int main(int ac, char **av) {
    FILE    *fp;
    char    buffer[132];

    fp = fopen("/home/user/end/.pass", "r");
    memset(buffer, 0, sizeof(buffer));

    if (!fp || ac != 2)
        return -1;

    fread(buffer, 1, 66, fp);
    buffer[65] = 0;
    buffer[atoi(av[1])] = 0;
    fread(&buffer[66], 1, 65, fp);
    fclose(fp);

    if (!strcmp(buffer, av[1]))
        execl("/bin/sh", "sh", 0);
    else
        puts(&buffer[66]);
    
    return 0;
}
```

The code is extremely straightforward. The code uses a single buffer to store 2 elements. It first opens the password file of end user, and store the password in the firsts 66 characters of that buffer. It then uses atoi on av[1] and set the buffer[result] to '\0'. Then it reads again 65 characters and stores it into the second part of the buffer starting at buffer[66]. If buffer is equal to av[1] then we get a shell as end user, else it just print the buffer after 66 charaters.

### A very quick solution

When executing the code and trying some parameters, we can see that the code simply prints a '/n' and that's it. Where it is supposed to print the end of the password file of end user. Maybe it means that the file is not that long, so nothing is stored in this second part of the buffer. The password is still stored in the first part of the buffer.

This coe is actually very weird, using av[1] as a pointer location but also a comparison with the passwrd, as if the code wanted to use av[1] and av[2] but was made by a rookie. In any case a big mistake is done on this line:

```c
buffer[atoi(av[1])] = 0;
```

Because if we write 0, it means the buffer[0] will be set at '\0', therefore the string is non existing and 0 size long. buffer doesn't store anything anymore from the code perspective. It does not solve the level tho, since the comparison between av[1] and buffer won't work, one is equal to "0", the other one to nothing. However atoi() has a weird property that we can read in the man, in the bugs section:

```
errno is not set on error so there is no way to distinguish
between 0 as an error and as the converted value.
```

If an error occurs in atoi, 0 will be returned, and the code won't be able to distinguish of this being an error, or being an actual 0 value, and will use it as a 0 for the rest of the code. So we tried to provoke an error in atoi by sending a null string, this way av[1] would be equal to nothing, and compared to buffer set at '\0', so nothing too, and we got the shell:

```sh
bonus3@RainFall:~$ ./bonus3 ""
$ whoami
end
$ cat /home/user/end/.pass
3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
```
