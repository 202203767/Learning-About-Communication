#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

void print_endian(unsigned int data) {
    unsigned char *p = (char *)&data;
    for(int i = 0; i < sizeof(data); i++) {
        printf("+%d : %02X\n", i, p[i]);
    }
}

int main(void) {
    unsigned int data = 0x12345678;

    data = htonl(data);
    print_endian(data);
    printf("\n");
    data = ntohl(data);
    print_endian(data);

    return 0;
}