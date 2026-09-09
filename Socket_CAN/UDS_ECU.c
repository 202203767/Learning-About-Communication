#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define WAIT_TIME 2000000 //マイクロ単位
#define MAX_SOCKET 1

typedef struct {
    uint8_t sid;
    uint8_t did_high;
    uint8_t did_low;

    uint8_t data_high;
    uint8_t data_low;
} UDS_receive;

int main(void) {
    UDS_receive receive;
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame UDS_responce, UDS_data;

    //ソケットの生成
    can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(can_socket < 0) {
        printf("socket error\n");
        exit(1);
    }

    //インターフェースの取得
    strcpy(network_setup.ifr_name, interface);
    ioctl(can_socket, SIOCGIFINDEX, &network_setup);

    //バインド
    addr.can_family = AF_CAN;
    addr.can_ifindex = network_setup.ifr_ifindex;
    bind(can_socket, (struct sockaddr *)&addr, sizeof(addr));

    while(1){
        printf("wating・・・\n");
        read(can_socket, &UDS_data, sizeof(UDS_data));
        printf("receive data success!");
        usleep(WAIT_TIME);
        printf("ID:%X\n", UDS_data.can_id);
        printf("DLC:%X\n", UDS_data.can_dlc);
        for(int i = 0; i < UDS_data.can_dlc; i ++) {
            printf("%X\n", UDS_data.data[i]);
        }
        switch(UDS_data.data[0]) {
            case 0x10: {
                printf("Diagnostic Session Control\n");
                write(can_socket, &UDS_responce, sizeof(UDS_responce));
                break;
            }
            case 0x11: {
                printf("ECU Reset\n");
                write(can_socket, &UDS_responce, sizeof(UDS_responce));
                break;
            }
            case 0x22: {
                printf("Read Data By Identifier\n");
                receive.sid = 0x62;
                receive.did_high = 0xF1;
                receive.did_low = 0x90;
                receive.data_high = 0x12;
                receive.data_low = 0x34;
                UDS_responce.can_id = 0x7E8;
                UDS_responce.can_dlc = 5;
                UDS_responce.data[0] = receive.sid;
                UDS_responce.data[1] = receive.did_high;
                UDS_responce.data[2] = receive.did_high;
                UDS_responce.data[3] = receive.data_high;
                UDS_responce.data[4] = receive.data_low;
                write(can_socket, &UDS_responce, sizeof(UDS_responce));
            }
            case 0x2E: {
                printf("Write Data By Identifier\n");
                write(can_socket, &UDS_responce, sizeof(UDS_responce));
                break;
            }
            case 0x19: {
                printf("Read DTC Information\n");
                write(can_socket, &UDS_responce, sizeof(UDS_responce));
                break;
            }
        }
    }
    return 0;
}