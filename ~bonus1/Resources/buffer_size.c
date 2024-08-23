#include <stdlib.h>
#include <stdio.h>

int main(int ac, char **av) {
    int nbr;

    if (ac < 2) {
        printf("Usage: %s <size_wanted>\n", av[0]);
        return 1;
    }

    nbr = atoi(av[1]);

    printf("Searching for the closest buffer size for: %d\n", nbr);
    for (int i = -536870912; i > -2147483648; i--) {
        if (i * 4 <= nbr && i * 4 + 5 > nbr + 1)
            printf("Buffer x*4 of size: %d with x = %d\n", i * 4, i);
    }
}
