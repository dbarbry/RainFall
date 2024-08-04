#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef void(*ptr)(void);

void    n(void) {
    system("/bin/cat /home/user/level7/.pass");
}

void    m(void) {
    printf("Nope");
    return;
}

int main(int ac, char **av) {
    char    *buffer;
    ptr     *func_ptr;

    buffer = (char *)malloc(0x40);
    func_ptr = (void *)malloc(4);
    *func_ptr = (void *)m;
    strcpy(buffer, av[1]);
    (**func_ptr)();
    return 0;
}
