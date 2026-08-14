#include <stdio.h>
#include <stdlib.h>

#define MAX_MESSEGE (128)

int main(void) {
    char message[MAX_MESSEGE];
    int count = 1;
    FILE *write = fopen("send.txt", "w");
    if(write == NULL) {
        printf("メッセージを書けません\n");
        exit(1);
    }

    printf("input messege\n");
    while(1) {
        printf("%d:", count);
        fgets(message, sizeof(message), stdin);
        if(message[0] == '\n') {
            break;
        }
        fputs(message, write);
        count++;
    }
    fclose(write);
    printf("send message!\n");

    return 0;
}