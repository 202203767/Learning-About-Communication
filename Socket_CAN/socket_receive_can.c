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
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame receive;
    uint16_t send_id[] = {SPEED, OIL_TEMP, WATER_TEMP, RPM};

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

    while(1) {
        read(can_socket, &receive, sizeof(receive));
        switch(receive.can_id) {
            case SPEED:
                uint16_t speed_int = ((uint16_t)receive.data[0] << 8) | receive.data[1];
                double speed = speed_int / 10.0;
                printf("%.1f km/h\n", speed);
                break;
            case OIL_TEMP:
                uint8_t oil_temp = receive.data[1];
                printf("%d C\n", oil_temp);
                break;
            case WATER_TEMP:
                uint8_t water_temp = receive.data[1];
                printf("%d C\n", water_temp);
                break;
            case RPM:
                uint16_t rpm = ((uint16_t)receive.data[0] << 8) | receive.data[1];
                printf("%d\n", rpm);
                break;
        }
    }
    return 0;
}