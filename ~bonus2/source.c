#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int language;

int greetuser(char *src) {
    char    dest[64];

    if ( language == 1 ) {
        (dest, "\x48\x79\x76\xc3\xa4\xc3\xa4\x20\x70\xc3\xa4\x69\x76\xc3\xa4\xc3\xa4\x20", 13);
    }
    else if (language == 2) {
        strncpy(dest, "Goedemiddag! ", 13);
    }
    else {
        strncpy(dest, "Hello ", 7);
    }

    strcat(dest, src);
    return puts(dest);
}

int main(int ac, char **av) {
    char    buffer[76];
    char    buffer_final[76];
    char    *lang;

    if (ac != 3)
        return 1;

    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, av[1], 40);
    strncpy(buffer + 40, av[2], 32);
    lang = getenv("LANG");

    if (lang) {
        if (!memcmp(lang, "fi", 2)) {
            language = 1;
        } else if (!memcmp(lang, "nl", 2)) {
            language = 2;
        }
    }

    memcpy(buffer_final, buffer, sizeof(buffer_final));

    return greetuser(buffer);
}
