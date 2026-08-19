#include <stdio.h>
#include <stdint.h>

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

void send_lin_frame(LIN *frame, int len){
    printf("Break : %02X\n",frame->break_field);
    printf("Sync  : %02X\n",frame->sync_byte_field);
    printf("PID   : %02X\n",frame->protected_indefinder_field);
    for(int i = 0; i < len; i++){
        printf("DATA[%d] : %02X\n",i,frame->data[i]);
    }
    printf("Checksum : %02X\n",frame->checksum);
}

uint8_t check_sum(uint8_t data[], int len) {
    uint8_t checksum = 0;
    for(int i = 0; i < len; i ++) {
        checksum += data[i];
    }
    return ~checksum;
}

int main(void) {
    LIN sample;
    int len = 2;//バイト数今回は車速を挿入するため2バイトとする
    uint16_t spped = 1378;//車速今回は137.8km/hとする

    //それぞれの情報を挿入
    sample.break_field = BREAK_FIELD;
    sample.sync_byte_field = LIN_SYNC;
    sample.protected_indefinder_field = PID_SPEED;
    sample.data[0] = (spped >> 8) & 0xFF;
    sample.data[1] = spped & 0xFF;
    sample.checksum = sample.protected_indefinder_field + check_sum(sample.data,len);

    //配列に置いて送信
    unsigned char send_data[sizeof(sample)];
    send_data[0] = sample.break_field;
    send_data[1] = sample.sync_byte_field;
    send_data[2] = sample.protected_indefinder_field;
    for(int i = 0; i < len; i ++) {
        send_data[i + 3] = sample.data[i];
    }
    send_data[len + 3] = sample.checksum;

    send_lin_frame(&sample, len);

    return 0;
}