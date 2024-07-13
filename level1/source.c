#include <stdio.h>

int run(void) {
    fwrite("Good... Wait what?\n", 1, 0x13, stdout);
    system("/bin/sh");
    return 0;
}

int main(void) {
    char    buffer[76];

    gets(buffer);
    return 0;
}
