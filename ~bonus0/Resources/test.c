#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int ac, char **av) {
    printf("%p", getenv(av[1]));
    return 0;
}
