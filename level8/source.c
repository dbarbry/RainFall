#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char    *auth = NULL;
char    *service = NULL;

int main(void) {
    char    buffer[128]; // Correct size ?

    while (1) {
        printf("%p, %p \n", auth, service);
    
        if (!fgets(buffer, 128, stdin))
            break;

        if (!memcmp(buffer, "auth ", 5)) {
            auth = (char *)malloc(4);
            *auth = 0;
            if (strlen(buffer + 5) <= 30)
                strcpy(auth, buffer + 5);
        }

        if (!memcmp(buffer, "reset", 5))
            free(auth); // no check if auth is defined

        if (!memcmp(buffer, "service", 6)) // 6 instead of 7 ?
            service = (int)strdup(buffer + 7);

        if (!memcmp(buffer, "login", 5)) {
            if (auth[32] == 0) // no check if auth is defined, some decompiler use *(auth + 32)
                fwrite("Password:\n", 1, 10, stdout);
            else
                system("/bin/sh");
        }
    }

    return 0;
}
