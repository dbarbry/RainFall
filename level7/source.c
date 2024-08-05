#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Can't compile in 64bits machine

char    c[68];

void m(void) {
  printf("%s - %d\n", c, (int)time(0));
  return;
}

int main(int ac, char **av) {
    int     *addr1;
    int     *addr2;
    void    *tmp;
    FILE    *stream;

    addr1 = malloc(8);
    *addr1 = 1;
    tmp = malloc(8);
    addr1[1] = (int)tmp;

    addr2 = malloc(8);
    *addr2 = 2;
    tmp = malloc(8);
    addr2[1] = (int)tmp;

    strcpy((char *)addr1[1], av[1]);
    strcpy((char *)addr2[1], av[2]);

    stream = fopen("/home/user/level8/.pass", "r");
    fgets(c, 68, stream);
    puts("~~");
    return 0;
}
