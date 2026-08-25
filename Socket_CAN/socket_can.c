#include <stdio.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int can_socket = 0;
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame send;
    struct can_frame receive;

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
    send.can_id = 0x100;
    send.can_dlc = 2;
    send.data[0] = 0x05;
    send.data[1] = 0x62;
    printf("send start\n");
    write(can_socket, &send, sizeof(send));
    return 0;
}