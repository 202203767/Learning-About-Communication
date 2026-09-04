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

int main(void) {
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct canfd_frame receive;

    //ソケットの生成
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

    while(1) {
        //read(can_socket, &receive, sizeof(receive));
        uint16_t speed_int = ((uint16_t)receive.data[1] << 8) | receive.data[0];
        double speed = speed_int / 10.0;
        printf("%.1f km/h\n", speed);
        uint8_t oil_temp = receive.data[2];
        printf("%d C\n", oil_temp);
        uint8_t water_temp = receive.data[3];
        printf("%d C\n", water_temp);
        uint16_t rpm = ((uint16_t)receive.data[5] << 8) | receive.data[4];
        printf("%d\n", rpm);
    }
    return 0;
}