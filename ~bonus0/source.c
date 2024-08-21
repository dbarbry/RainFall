#include <stdio.h>
#include <string.h>

void p(char *ppBuffer, char *message) {
    char    buffer[4096];

    puts(message);
    read(0, buffer, 4096);
    *strchr(buffer, '\n') = 0;
    strncpy(ppBuffer, buffer, 20);

    return;
}

void pp(char *mainBuffer) {
    char    buffer1[20];
    char    buffer2[20];
    size_t  len;

    p(buffer1, " - ");
    p(buffer2, " - ");

    strcpy(mainBuffer, buffer1);

    len = strlen(mainBuffer);
    mainBuffer[len] = ' ';

    strcat(mainBuffer, buffer2);

    return;
}

int main(void) {
    char buffer[54];

    pp(buffer);
    puts(buffer);

    return 0;
}
