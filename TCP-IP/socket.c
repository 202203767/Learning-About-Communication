#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {
    int socket_make;
    struct sockaddr_in server_adder;

    socket_make = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_make < 0) {
        printf("ソケット作成に失敗\n");
        exit(EXIT_FAILURE);
    }

    server_adder.sin_family = AF_INET;
    server_adder.sin_port = htons(8080);
    server_adder.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(socket_make, (struct sockaddr *)&server_adder, sizeof(server_adder)) < 0) {
        printf("接続に失敗\n");
        close(socket_make);
        exit(EXIT_FAILURE);
    }

    printf("サーバーに接続\n");
    close(socket_make);
    return 0;
}