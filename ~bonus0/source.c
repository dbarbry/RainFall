#include <stdio.h>

void pp() {
    return;
}

void p() {
    return;
}

int main(void) {
    char buffer[54];

    pp(buffer);
    puts(buffer);
    return 0;
}
