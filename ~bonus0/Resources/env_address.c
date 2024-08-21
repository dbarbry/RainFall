#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void toEndian(uint32_t address) {
    uint8_t bytes[4];

    bytes[0] = (uint8_t)(address & 0xFF);
    bytes[1] = (uint8_t)((address >> 8) & 0xFF);
    bytes[2] = (uint8_t)((address >> 16) & 0xFF);
    bytes[3] = (uint8_t)((address >> 24) & 0xFF);

    printf("Lil-endian format: \\x%02x\\x%02x\\x%02x\\x%02x\n", bytes[0], bytes[1], bytes[2], bytes[3]);

    return;
}

int main(int ac, char **av) {
    uint32_t    address;

    if (ac < 2) {
        printf("Usage: %s <variable_name>\n", av[0]);
        return 1;
    }

    printf("Searching address of %s env variable:\n", av[1]);
    address = getenv(av[1]);
    printf("Big-endian format: %p\n", address);
    toEndian(address);

    return 0;
}
