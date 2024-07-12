# level2

(important to note, we work on this vm with a 32bit x86 architecture, therefore some parts are architecture dependents like addresses).

This level is the first really hard one, where we could solve level1 not knowing that much what we were doing besides understanding very basic buffer overflow knowledges, here we need to dive into it and understand how the class stack works, how asm works, what does ebp, eip, esp and others variables actually mean, and deeply understand the real potential of buffer overflow alongside the wide variety of method to reach a buffer overflow.

This readme will, because I want it, explain as well as possible how the call stack work, how everything we need to know for buffer overflows works and will probably go into way more than what this level basically wants us to do. But it will be very usefull for the rest of this project (I hope so).

The first thing we notice while analyzing this level2 file with ghidra is this function caLled in main:
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

We are gonna ignore some part for now, but there are three interesent parts of this code we can directly discuss about:
- The gets function, we know it, doesn't have bound restriction, is dangerous to use and can store more than the buffer size is.
- The if statement that checks if the address folowing the buffer starts by 0xb, if it's the case, it prints the address and exit.
- the strdup function that, we know, uses malloc so uses space on the heap and not on the stack this time. It simply make a copy of the buffer on the heap.

---

Where we will go into more details later for each components, let's make a quick reminded of how the memory works.
![Memory organization](https://github.com/kbarbry/RainFall/blob/main/other/1_oLatmtP-JHDF8SSpoX2Nxg.gif)


# Important doc

[Advanced exploit of buffer overflows](https://arxiv.org/pdf/cs/0405073)
[Memory and call stack](https://textbook.cs161.org/memory-safety/x86.html)
[Frame Pointer Overflow](https://bob3rdnewbie.tistory.com/188)
[x86 32bit structure](https://tirkarp.medium.com/understanding-x86-assembly-5d7d637efb5)