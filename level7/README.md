# level7

### First analysis

In this level we get a very heavy main and a little m() function not called anywhere as we can see here on this decompiled version of level7:

```c
void m(void *param_1,int param_2,char *param_3,int param_4,int param_5) {
  time_t tVar1;
  
  tVar1 = time((time_t *)0x0);
  printf("%s - %d\n",c,tVar1);
  return;
}

undefined4 main(undefined4 param_1,int param_2) {
  undefined4 *puVar1;
  void *pvVar2;
  undefined4 *puVar3;
  FILE *__stream;
  
  puVar1 = (undefined4 *)malloc(8);
  *puVar1 = 1;
  pvVar2 = malloc(8);
  puVar1[1] = pvVar2;
  puVar3 = (undefined4 *)malloc(8);
  *puVar3 = 2;
  pvVar2 = malloc(8);
  puVar3[1] = pvVar2;
  strcpy((char *)puVar1[1],*(char **)(param_2 + 4));
  strcpy((char *)puVar3[1],*(char **)(param_2 + 8));
  __stream = fopen("/home/user/level8/.pass","r");
  fgets(c,0x44,__stream);
  puts("~~");
  return 0;
}
```

To understand more what happens we can check source.c again, let's break down this code step by step:
- First it does some allocations on 2 different variables that we will call addr1 and addr2, addr1[0] = 1 and addr2[0] = 2, then on addr1[1] and addr2[1] we get a new allocation of 8 characters on both.
- It calls strcpy() two times, one to copy into addr1[1] the first argument (av[1]) and same thing with addr2[1] and av[2].
- level8 password file is opened in readonly and read by the program with fgets then stored into c, a global variable.
- Then we simply print two tildes in stdout.

Even if m() is never called we cant analyze it quicky too:
- First it calls the function time() that will gives us a timestamp (long number).
- Then it prints c and the timestamp.

So as we can see one part of the program reads level8's password, and another part that prints it, tho the part that prints it is not accessible. However if it's read, maybe there is some way to find into the stack or the heap this value somewhere, we just have to find how.

### Finding the solution
