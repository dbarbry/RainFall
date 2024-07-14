# level2

### Intro

(important to note, we work on this vm with a 32bit x86 architecture, therefore some parts are architecture dependents like addresses).

This level is the first difficult one, where we could solve level1 not knowing that much what we were doing besides understanding very basic buffer overflow knowledges, here we need to dive into it and understand how the class stack works, how asm works, what does ebp, eip, esp and others variables actually mean, and deeply understand the real potential of buffer overflow alongside the wide variety of method to reach a buffer overflow.

This readme will, because I want it, explains as well as possible how the call stack works, how everything we need to know for buffer overflows works and will probably go into way more than what this level basically wants us to do. But it will be very usefull for the rest of this project (I hope so).

### First steps

The first thing we notice while analyzing this level2 file with ghidra is this function called in main:

```c
void p(void)
{
  uint unaff_retaddr;
  char local_50 [76];
  
  fflush(stdout);
  gets(local_50);
  if ((unaff_retaddr & 0xb0000000) == 0xb0000000) {
    printf("(%p)\n",unaff_retaddr);
                    /* WARNING: Subroutine does not return */
    _exit(1);
  }
  puts(local_50);
  strdup(local_50);
  return;
}
```

We are gonna ignore some elements for now, but there are three interesting parts of this code we can directly discuss about:
- The gets function, we know it, doesn't have bound restriction, is dangerous to use and can store more than the buffer size is.
- The if statement that checks if the address folowing the buffer starts by 0xb, if it's the case, it prints the address and exit.
- the strdup function that, we know, uses malloc so uses space on the heap and not on the stack this time. It simply make a copy of the buffer on the heap.

### Memory structure

Where we will go into more details later for each components, let's make a quick reminder of how memory works.

---

![Memory organization](https://github.com/kbarbry/RainFall/blob/main/other/1_oLatmtP-JHDF8SSpoX2Nxg.gif)

As we can see the code is in the text part, and it calls the stack for functions, local variables etc... The stack is on the high address side of the memory (so closer to 0xffffffff addresses) and the code part (text) is on the low address part (so closer to 0x00000000 addresses). The heap is on this side too, so maloc() functions will allow memory on this area. In x86 32bit architecture on Linux, we can find the starting addresses for each sections:

```sh
0x08048000  code
0x08052000  data
0x0805A000  bss (zero data)
0x08072000  end of data (brk marker)
0xBFFFE000  stack
```

While values might change on different architecture, and even not be that correct because of vitual memory (that I defenitely don't understand anything yet), the important here is to see that the stack starts at 0xbfffe000 address. We now understand why level2 checks for 0xb... addresses, because simple overlow would call the stack at the buffer address to execute a shellcode or other malicious code.Or here the buffer we are gonna pass through gets function is going straight to the stack, therefore in the range of 0xb... addresses. This if statement is some sort of anti buffer-overflow method (and seems effective for the stack part as far as I know).

We can also check this using "info registers" command on gdb. Registers is a list of variables contained in the processor itself, for very fast access and changes of values. This is where we find the different variables we see everywhere when searching on buffer overflow:
- EIP - (Instruction Pointer) Stores the next instruction address (or the current one depends on the architecture used), so is incremented after each line of code.
- ESP - (Stack Pointer) Stores the current stack pointer and changes after each iteration ongoing in the stack. It is the top of the stack at all time.
- EBP - (Base Pointer) Stores the last calling function return address (called SFP/Saved Frame Pointer). We can also say that it is the address of the top stack frame. Each function called create a new stack frame.
*In here the E at the beggining of each register variable stands for Extended, name for 32bit, would be R for Register on 64bit or was absent on 16bit.*

In order to be sort of exhaustive, we also have EDI and ESI for string operations, and others like EDX, ECX, EBX or EAX that are really not useful here.

To understand a little more here is the prologue we can see before every function in assembly (ASM,x86):

```asm
PUSH    EBP
MOV     EBP, ESP
SUB     ESP, 0Ch

MOV     DWORD [EBP-4], 0 ; x
MOV     DWORD [EBP-8], 1 ; y
MOV     DWORD [EBP-0Ch], 1 ;z
```

This is what it does line by line:
- Push EBP on the stack (old EBP is now stored at the top of the stack, ESP is incremented to the top of the stack too), this is the Saved Frame Pointer (SFP).
- Set EBP at ESP, so now EBP = ESP = top of the stack both.
- Allocate memory on the stack for entry variable (parameters), 0Ch is ofc a size that has is variable depending on the number of entry arguments, here 3, moving ESP down a few address (incrementing the top of the stack, remember stack goes from higher to lower addresses).
- For those three next line it really depends on the function called, here we have 3 parameters in the function called so we move their value respectively in EBP-4, EBP-8 and EBP-0Ch, we take EBP cause ESP is always changing for the top of the stack, where EBP stays at its last set state, during line 3 operation, so it is still equal to the old EBP address in the stack. We substract addresses cause again, the stack goes from higher to lower addresses, so substracting 4 is going closer to the top of the stack.
*It is not shown here but I also saw before saving EBP address on the first line to also push EIP address, so the stack saves both EIP and EBP old state*

### Resolution of level2

If we pay attention to how we solved level1, we can see that we overflowed the buffer by filling it with 76 chars then the return address. Or here if we test that with an address starting with 0xb..., the expected result would be the program to print the address and then exit. Or when doing so the program just run normally, then crash with a segfault error.

Thanks to the explanations above, we can understand why.

Which didn't seem to be the case in level1, level2 seems to have a SFP (Saved Frame Pointer), which we know takes 4 bytes (size of a regular address in 32bit architecture) and then the return address. We then have in bytes:

```
|          buffer(76)          |   SFP (4)   | Ret_addr (4) |
```

Indeed if we now try to inject 4 more bytes before the address this is what we get:

```bash
level2@RainFall:~$ echo $(python -c "print('\x90'*76+'\x90'*4+'\x44\x84\x04\xb8')") > /tmp/payload
level2@RainFall:~$ cat /tmp/payload -| ./level2 
(0xb8048444)
...
```

Where we could be happy to see that, it actually is a bad thing. In order to solve level2 with a basic buffer overflow, the idea is to inject after the NOP (\x90) a shellcode, then change the return address to beginning of the buffer, so the shellcode is executed and we get a shell as level3. This attack is a return-to-stack attack.
*A shellcode is a piece of code that launch a shell and is generally used for buffer overflow exploits, the shorter the better, so it can fits in very small buffer.*

Problem here, the stack starts at the address 0xbfffe000 and as we explained it earlier, the if statement checking if an address starts with 0xb acts as an anti buffer overflow.

We have to find something else.

Buffer overflows are possible in every section of the memories with differents methods. Here the only other sections that the program seems to use and that could have a buffer overflow is the heap.

Indeed the strdup() function as we can read on the man:

```
The strdup() function returns a pointer to a new string which is a duplicate of the string s. Memory for the new string is obtained with malloc(3), and can be freed with free(3).
```

malloc() use the heap section of memory which is located in a completely different area as the stack (at the beginning of the addresses) and is then not gonna be caught by the anti buffer overflow checking if the return address is 0xb.

### What is the heap ? And malloc ?

The heap is basically a big chunck of memory available in case we need to allocate more space for vairables in our code. We very often instantly talk about malloc and free when talking about the heap but those functions actually comes from the libc and uses mmap() and brk(). We are still gonna focus on malloc because most of the libc functions uses it and strdup is no exception. Also to be more precise we are gonna go deeper in the dlmalloc (dl coming from the name of the person who codes this version of malloc) which is the most comonly used.

## Important doc

[Advanced exploit of buffer overflows](https://arxiv.org/pdf/cs/0405073)

[Memory and call stack](https://textbook.cs161.org/memory-safety/x86.html)

[Frame Pointer Overflow](https://bob3rdnewbie.tistory.com/188)

[x86 structure #1](https://tirkarp.medium.com/understanding-x86-assembly-5d7d637efb5)

[Call stack ESP explained](https://www.youtube.com/watch?v=RU5vUIl1vRs)

[ASM Prologue understanding](https://www.youtube.com/watch?v=nbZJOT1gKX4)

[Overflow deep explaination](http://theamazingking.com/tut1.php) !NOT HTTPS

[Shellcode DataBase](https://shell-storm.org/shellcode/index.html)
