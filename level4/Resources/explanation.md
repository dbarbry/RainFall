### How to optimize the payload

The basic idea is instead of using "%n", using "%hn", which is half a 32bit address long, also called a short. it is 4 bites long instead of 8, which means we could write in the address of m the firsts 4 bytes, and then at m+2 the 4 last bytes. By doing so we will have to write two small sets of spaces instead of 1 huge set of space since the number of spaces increases exponentially to the number of bytes we have to write.

So first we know that m address is 0x08049810, so m+2 address is 0x08049812. In little endian format we have:
- 0x08049810 = \x10\x98\x04\x08
- 0x08049812 = \x12\x98\x04\x08

Then we have the value we want to inject to m in order to solve the level which is 0x01025544, so divided in two and in decimal form we have:
- 0102 - high order bytes - 258   in decimal
- 5544 - low order bytes  - 21828 in decimal

Important to say, in this method we are going to use 2 times "%n", so we have to write the lowest decimal score first, so we will first write the high order bytes, and then the low order bytes (the buffer is increasing, we can't write a lower size of buffer with %n the second one, wouldn't make sense).

So our buffer is going to look like this so far:

```py
'\x12\x98\x04\x08' + '\x10\x98\x04\x08' + '%hn' + '%hn'
```

Then, as we did originally in level3 and level4 without optimization, we want to move the stack pointer of printf to fall precisely where the buffer memory space starts, so when calling the first %hn, we land on the address of m, and when using the second %hn we land on the second part of m. Let's add it:

```py
'\x12\x98\x04\x08' + '\x10\x98\x04\x08' + '%x' * 11 + '%hn' + '%hn'
```

Perfect. Now when executing the program doesn't segfault so it works so far, we are just not writing the correct value inside of m. Let's focus on the first address first, it is the high order bytes, we have to write a buffer of 258 chars in this first address. for that we know that we already write 8 bytes of addresses + 60 bytes for the '%x' * 11 (you have to print the stack to know that, you might not have the same). So we have 258 - 60 - 8 = 190, to write 190 more chars, we are going to take the 11th %x we wrote, and replace it with a %x with padding containing the number of chars we want. Let's update our payload:

```py
'\x12\x98\x04\x08' + '\x10\x98\x04\x08' + '%x' * 10 + '%190x' + '%hn' + '%hn'
```

If we check on gdb the value of m with this payload we have this:

```sh
(gdb) p m
$1 = 16908546
```

That's a very weird number but if we convert it in hexadecimal we get 0x1020102, since we print the same length in both addresses which is 0102 for the high order bytes we are aiming for, it makes complete sense. So everything is perfect so far, we just need to write the lower order bytes. For that same method, we add a %x with the padding size we want, so for the lower order bytes we wanted to print 21828 char, we remove to it the 258 bytes long already printed by the first part of the buffer, and we get 21570 bytes. Let's update the payload:

```py
'\x12\x98\x04\x08' + '\x10\x98\x04\x08' + '%x' * 10 + '%190x' + '%hn' + '%21570x' + '%hn'
```

However when executing it, this time the program segfault. Which makes complete sense too, since we print a new %x in between the two '%hn', the address of the lower order bytes of n, is consumed and printed as a %x and then %hn try to print the result in a random address that is probably not available. To fix that we can just add 'AAAA' or any other group of 4 characters between the two addresses. By doing so the %21570x parameter will print AAAA with a huge paddin, and then comes the address of the low order bytes in the stack that will be used by %hn. Let's not forget to remove 4 to the padding of the high order bytes since we added 4 bytes to the payload and here we are:

```py
'\x12\x98\x04\x08' + 'AAAA' +'\x10\x98\x04\x08' + '%x' * 10 + '%186x' + '%hn' + '%21570x' + '%hn'
```

In gdb the value is at:

```sh
(gdb) p m
$1 = 16930116
```

By doing this method, we printed 22.086 characters instead of 16.930.116 characters. That makes a decent optimization of 99.87 %. Not bad.
