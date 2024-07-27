#include <stdio.h>
#include <stdlib.h>

void    o(void) {
    system("/bin/sh");

    exit(1);
}

void    n(void) {
    char    buffer[520];

    fgets(buffer, 512, stdin);
    printf(buffer);

    exit(1);
}

int main(void) {
    n();
    return 0;
}