#include <stdio.h>
#define MAX_DATA 8

typedef struct data {
    unsigned char id;
    int data[MAX_DATA];
} data;

int main(void) {
    data sample_data;
    
    printf("構造体全体のバイト数は%zu\n", sizeof(sample_data));
    printf("%zu %p\n", sizeof(sample_data.id), &sample_data.id);
    printf("%zu %p\n", sizeof(sample_data.data), &sample_data.data);

    return 0;
}