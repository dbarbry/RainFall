#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int ac, char **av) {
    char    dest[40];
    int     size;

    size = atoi(av[1]);

    if (size < 10) {
        memcpy(dest, av[2], size * 4);
        if (size == 1464814662)
            execl("/bin/sh", "sh", 0); // path, args, env

    } else {
        return 1;
    }

    return 0;
}
