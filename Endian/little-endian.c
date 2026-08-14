#include <stdio.h>
#include <string.h>

int main(void) {
    int data = 0x12345678;//16進数表記

    char *p = (char *)&data;

    for(int i = 0; i < sizeof(data); i++) {
        printf("+%d : %02X\n", i, p[i]);
    }

    return 0;
}