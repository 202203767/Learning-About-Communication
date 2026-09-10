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
#define MAX_SEND 2

int main(void) {
    int num = 0;
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame VIN_request, VIN_receive;

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

    printf("Preparing the request\n");
    VIN_request.can_id = 0x7E0;
    VIN_request.can_dlc = 3;
    VIN_request.data[0] = 0x22;
    VIN_request.data[1] = 0xF1;
    VIN_request.data[2] = 0x90;

    printf("Send request\n");
    usleep(WAIT_TIME);
    write(can_socket, &VIN_request, sizeof(VIN_request));
    printf("send request success!\n");

    printf("First frame received\n");
    read(can_socket, &VIN_receive, sizeof(VIN_receive));
    usleep(WAIT_TIME);
    printf("ID:%X\n", VIN_receive.can_id);
    printf("DLC:%X\n", VIN_receive.can_dlc);
    for(int i = 0; i < VIN_receive.can_dlc; i ++) {
        printf("%X ", VIN_receive.data[i]);
    }
    printf("\n");

    printf("Send Flow control\n");
    VIN_request.can_id = 0x7E0;
    VIN_request.can_dlc = 3;
    VIN_request.data[0] = 0x30;
    VIN_request.data[1] = 0x00;
    VIN_request.data[2] = 0x00;
    printf("Send request\n");
    usleep(WAIT_TIME);
    write(can_socket, &VIN_request, sizeof(VIN_request));
    printf("Send request success!\n");

    int n = 0;

    while(n < 2) {
        printf("Receive data\n");
        read(can_socket, &VIN_receive, sizeof(VIN_receive));
        usleep(WAIT_TIME);
        printf("ID:%X\n", VIN_receive.can_id);
        printf("DLC:%X\n", VIN_receive.can_dlc);
        for(int i = 0; i < VIN_receive.can_dlc; i ++) {
            printf("%X ", VIN_receive.data[i]);
        }
        printf("\n");
        n ++;
    }
    return 0;
}