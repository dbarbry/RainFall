#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int ac, char **av) {
    FILE    *fp;
    char    buffer[132];

    fp = fopen("/home/user/end/.pass", "r");
    memset(buffer, 0, sizeof(buffer));

    if (!fp || ac != 2)
        return -1;

    fread(buffer, 1, 66, fp);
    buffer[65] = 0;
    buffer[atoi(av[1])] = 0;
    fread(&buffer[66], 1, 65, fp);
    fclose(fp);

    if (!strcmp(buffer, av[1]))
        execl("/bin/sh", "sh", 0);
    else
        puts(&buffer[66]);
    
    return 0;
}
