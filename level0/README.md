# level0

On this levl0 we notice an executable called level0 made by level1 that segfault, if executed with no argument, and print "No !" if an argument is passed. We copied the file locally and analyzed it with ghidra and amongst all the decompiled code we can find:

```c
iVar1 = atoi(*(char **)(param_2 + 4));

if (iVar1 == 0x1a7) {
    [...]
}
else {
    fwrite("No !\n",1,5,(FILE *)stderr);
}
```

So we can quickly understand what we have to convert 0x1a7 in decimal which is 423 and then do:

```sh
level0@RainFall:~$ ./level0 423
$ whoami
level1
$ cd /home/user/level1
$ cat .pass
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
```
