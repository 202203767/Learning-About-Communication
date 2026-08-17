#include <stdio.h>
#define MAX_DATA 64

typedef struct data {
    int id;
    char data[MAX_DATA];
} data;

int main(void) {
    data sample_data;
    printf("%zu\n", sizeof(sample_data));
    printf("%zu\n", sizeof(sample_data.id));
    printf("%zu\n", sizeof(sample_data.data));

    return 0;
}