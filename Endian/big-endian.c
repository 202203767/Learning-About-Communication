#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(void) {
    int data = 0x12345678;

    data = htonl(data);//
    char *p = (char *)&data;

    for(int i = 0; i < sizeof(data); i++) {
        printf("+%d : %02X\n", i, p[i]);
    }

    return 0;
}