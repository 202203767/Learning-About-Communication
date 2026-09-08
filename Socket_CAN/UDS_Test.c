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
} UDS_reqest;

int main(void) {
    UDS_reqest request;

    request.sid = 0x22;
    request.did_high = 0xF1;
    request.did_low = 0x90;
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame UDS_send, UDS_receive;

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

    UDS_send.can_id = 0x700;
    UDS_send.can_dlc = 3;

    UDS_send.data[0] = request.sid;
    UDS_send.data[1] = request.did_high;
    UDS_send.data[2] = request.did_low;

    for(int i = 0; i < MAX_SOCKET; i ++) {
        write(can_socket, &UDS_send, sizeof(UDS_send));

        read(can_socket, &UDS_receive, sizeof(UDS_receive));
    }
    return 0;
}