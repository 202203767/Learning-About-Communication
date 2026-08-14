#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAX_MESSAGE (64)

int socket_make;

void server_connect() {
    struct sockaddr_in server_adder;
    socket_make = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_make < 0) {
        printf("ソケット作成に失敗\n");
        exit(EXIT_FAILURE);
    }

    server_adder.sin_family = AF_INET;
    server_adder.sin_port = htons(35279);//ポート番号
    server_adder.sin_addr.s_addr = inet_addr("127.0.0.1");//IPアドレス

    if(connect(socket_make, (struct sockaddr *)&server_adder, sizeof(server_adder)) < 0) {
        printf("接続に失敗\n");
        close(socket_make);
        exit(EXIT_FAILURE);
    }

    printf("サーバーに接続\n");
}

int main(void) {
    char message[] =
    "GET / HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "\r\n";
    char receive[MAX_MESSAGE];

    server_connect();
    send(socket_make, message, strlen(message), 0);

    recv(socket_make, receive, sizeof(receive), 0);
    printf("受信\n");
    printf("%s\n", receive);

    close(socket_make);
    return 0;
}