#include <stdio.h>

#define MAX_MESSEGE (128)

int main(void) {
    char message[MAX_MESSEGE];
    FILE *read = fopen("send.txt", "r");

    printf("receive message\n");
    while(fgets(message, sizeof(message), read)) {
        printf("%s", message);
    }
    fclose(read);

    return 0;
}