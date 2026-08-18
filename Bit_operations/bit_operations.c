#include <stdio.h>
#include <stdint.h>

int main(void) {
    uint8_t data = 0xAA;

    //0xAAをそのまま表示
    printf("Defalt\n");
    printf("10進数%d\n", data);
    printf("8進数%o\n", data);
    printf("16進数%X\n", data);
    //ANDを導入
    printf("AND\n");
    printf("10進数%d\n", data & 0x0F);
    printf("8進数%o\n", data & 0x0F);
    printf("16進数%X\n", data & 0x0F);
    //ORを導入
    printf("OR\n");
    printf("10進数%d\n", data | 0x0F);
    printf("8進数%o\n", data | 0x0F);
    printf("16進数%X\n", data | 0x0F);
    //左シフト演算
    printf("Left shift operation\n");
    printf("10進数%d\n", data << 3);
    printf("8進数%o\n", data << 3);
    printf("16進数%X\n", data << 3);
    //右シフト演算
    printf("Right shift operation\n");
    printf("10進数%d\n", data >> 3);
    printf("8進数%o\n", data >> 3);
    printf("16進数%X\n", data >> 3);
    return 0;
}