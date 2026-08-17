#include <stdio.h>
#define MAX_DATA 8
#define MAX_SEND 10

typedef struct send_data {
    unsigned int id;
    unsigned char dlc;
    unsigned char data[MAX_DATA];
} send_data;

int main(void) {
    send_data example_data = {
        .id = 100,
        .dlc = 8, 
        .data = {0x11, 0x88, 0x77, 0x10, 0x22, 0x89, 0x12, 0x70},
    };
    unsigned char frame[MAX_SEND];

    //ここから送信するのに一回配列にすべて格納する
    frame[0] = example_data.id;
    frame[1] = example_data.dlc;
    for(int i = 0; i < example_data.dlc; i ++) {
        frame[i + 2] = example_data.data[i];
    }

    //ここでデータを送信
    //send(sock, &frame, sizeof(frame), 0);

    //データを受信
    send_data receive_data;
    //ここでデータを構造体に格納
    receive_data.id = frame[0];
    receive_data.dlc = frame[1];
    for(int i = 0; i < receive_data.dlc; i ++) {
        receive_data.data[i] = frame[i + 2];
    }

    return 0;
}