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

#define SET 0x200
#define MAX_SOCKET 2
#define WAIT_TIME 2000000 //マイクロ単位


int main(void) {
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct canfd_frame send;
    
    //速度・油温・水温・回転数
    double speed = 120.8;
    uint16_t speed_int = speed * 10;
    uint8_t oil_temp = 80;
    uint8_t water_temp = 90;
    uint16_t rpm = 4320;

    can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(can_socket < 0) {
        printf("socket error\n");
        exit(1);
    }

    //CAN FDを有効にする
    int enable_canfd = 1;
    setsockopt(can_socket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd));

    //インターフェースの取得
    strcpy(network_setup.ifr_name, interface);
    ioctl(can_socket, SIOCGIFINDEX, &network_setup);

    //バインド
    addr.can_family = AF_CAN;
    addr.can_ifindex = network_setup.ifr_ifindex;
    bind(can_socket, (struct sockaddr *)&addr, sizeof(addr));

    for(int i = 0; i < MAX_SOCKET; i ++) {
        send.can_id = SET;
        send.len = 6;

        //車速
        send.data[0] = speed_int & 0xFF;
        send.data[1] = (speed_int >> 8) & 0xFF;

        //油温
        send.data[2] = oil_temp;

        //水温
        send.data[3] = water_temp;

        //回転数
        send.data[4] = rpm & 0xFF;
        send.data[5] = (rpm >> 8) & 0xFF;
        
        printf("send speed\n");
        write(can_socket, &send, sizeof(send));
        usleep(WAIT_TIME);
    }
    return 0;
}