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

#define SPEED 0x100
#define OIL_TEMP 0x101
#define WATER_TEMP 0x102
#define RPM 0x103
#define ID_TYPES 3
#define MAX_SOCKET 1
#define WAIT_TIME 2000000 //マイクロ単位

int main(void) {
    int can_socket = 0;
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame send;
    struct can_frame receive;
    uint16_t send_id[] = {SPEED, OIL_TEMP, WATER_TEMP, RPM};
    //速度・油温・水温・回転数
    double speed = 120.8;
    uint16_t speed_int = speed * 10;
    uint8_t oil_temp = 80;
    uint8_t water_temp = 90;
    uint16_t rpm = 4320;

    //ソケットの生成
    can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(can_socket < 0) {
        printf("socket error\n");
        exit(1);
    }
    //インターフェースの取得
    strcpy(network_setup.ifr_name, "vcan0");
    ioctl(can_socket, SIOCGIFINDEX, &network_setup);
    //バインド
    addr.can_family = AF_CAN;
    addr.can_ifindex = network_setup.ifr_ifindex;
    bind(can_socket, (struct sockaddr *)&addr, sizeof(addr));
    //送信

    for(int i = 0; i < MAX_SOCKET; i ++) {
        for(int i = 0; i < ID_TYPES; i ++) {
            send.can_id = send_id[i];
            send.can_dlc = 2;
            switch(send.can_id) {
                case SPEED:
                    send.data[0] = (speed_int >> 8) & 0xFF;
                    send.data[1] = speed_int & 0xFF;
                    printf("send speed\n");
                    write(can_socket, &send, sizeof(send));
                    usleep(WAIT_TIME);
                    break;
                case OIL_TEMP:
                    send.data[0] = (oil_temp >> 8) & 0xFF;
                    send.data[1] = oil_temp & 0xFF;
                    printf("send oil temp\n");
                    write(can_socket, &send, sizeof(send));
                    usleep(WAIT_TIME);
                    break;
                case WATER_TEMP:
                    send.data[0] = (water_temp >> 8) & 0xFF;
                    send.data[1] = water_temp & 0xFF;
                    printf("send water temp\n");
                    write(can_socket, &send, sizeof(send));
                    usleep(WAIT_TIME);
                    break;
                case RPM:
                    send.data[0] = (rpm >> 8) & 0xFF;
                    send.data[1] = rpm & 0xFF;
                    printf("send rpm\n");
                    write(can_socket, &send, sizeof(send));
                    usleep(WAIT_TIME);
                    break;
            }
        }
    }
    return 0;
}