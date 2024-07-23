#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

void    p(void) {
    char buffer[76];
    void	*ret_addr;

    fflush(stdout);
    gets(buffer);

    ret_addr = __builtin_return_address (0); 

    if (((unsigned int)ret_addr & 0xb0000000) == 0xb0000000) {
        printf("(%p)\n", (void *)ret_addr);
        exit(1);
    }

    puts(buffer);
    strdup(buffer);

    return;
}

int main(void) {
    p();
    return 0;
}