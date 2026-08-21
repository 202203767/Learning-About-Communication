#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_DATA 8
#define LIN_SYNC 0x55
#define BREAK_FIELD 0x00
#define PID_SPEED 0x01

//LINフレーム
typedef struct LIN {
    uint8_t break_field;
    uint8_t sync_byte_field;
    uint8_t protected_indefinder_field;
    uint8_t data[MAX_DATA];
    uint8_t checksum;
} LIN;

void print_LIN(LIN LIN, int len) {
    printf("Break filed:%d\n", LIN.break_field);
    printf("Sync byte field:%d\n", LIN.sync_byte_field);
    printf("Protected indefinder field:%d\n", LIN.protected_indefinder_field);
    for(int i = 0; i < len; i ++) {
        printf("Data[%d]:%d\n", i, LIN.data[i]);
    }
    printf("Checksum:%d\n", LIN.checksum);
}

void differences(uint8_t checksum, uint8_t data[], uint8_t PID, int len) {//誤りがあるか確認
    checksum = ~checksum;
    for(int i = 0; i < len; i ++) {
        checksum -= data[i];
    }
    checksum -= PID;
    if(checksum == 0) {
        printf("誤りなし\n");
    } else {
        printf("誤りあり\n");
        exit(1);
    }
}

void send_and_receive(unsigned char sample_send[], int len){//送信したデータを受信した
    LIN sample_receive;
    
    sample_receive.break_field = sample_send[0];
    sample_receive.sync_byte_field = sample_send[1];
    sample_receive.protected_indefinder_field = sample_send[2];
    for(int i = 0; i < len; i ++) {
        sample_receive.data[i] = sample_send[i + 3];
    }
    sample_receive.checksum = sample_send[len + 3];
    differences(sample_receive.checksum, sample_receive.data, sample_receive.protected_indefinder_field, len);
    uint16_t speed_raw = (sample_receive.data[0] << 8) | sample_receive.data[1];
    double speed = speed_raw / 10.0;
    printf("receive data\n");
    print_LIN(sample_receive, len);
    printf("車速 : %.1f km/h\n", speed);
}

uint8_t make_check_sum(uint8_t data[],uint8_t PIF, int len) {//チェックサムを作成
    uint8_t checksum = PIF;
    for(int i = 0; i < len; i ++) {
        checksum += data[i];
    }
    return ~checksum;
}

int main(void) {
    LIN sample;
    int len = 2;//バイト数今回は車速を挿入するため2バイトとする
    double spped = 137.8;
    uint16_t input_spped = spped * 10;

    //それぞれの情報を挿入
    sample.break_field = BREAK_FIELD;
    sample.sync_byte_field = LIN_SYNC;
    sample.protected_indefinder_field = PID_SPEED;
    sample.data[0] = (input_spped >> 8) & 0xFF;
    sample.data[1] = input_spped & 0xFF;
    sample.checksum = make_check_sum(sample.data, sample.protected_indefinder_field, len);

    printf("send data\n");
    print_LIN(sample, len);

    //配列に置いて送信
    uint8_t send_data[len + 4];
    send_data[0] = sample.break_field;
    send_data[1] = sample.sync_byte_field;
    send_data[2] = sample.protected_indefinder_field;
    for(int i = 0; i < len; i ++) {
        send_data[i + 3] = sample.data[i];
    }
    send_data[len + 3] = sample.checksum;

    send_and_receive(send_data, len);//送信（仮定）

    return 0;
}