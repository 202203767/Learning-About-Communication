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

int main(void) {
    char vin[] = "NISSAN123456789AB";
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame VIN_responce, VIN_data;

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

    printf("wating・・・\n");
    read(can_socket, &VIN_data, sizeof(VIN_data));
    printf("Receive data success!\n");
    usleep(WAIT_TIME);
    printf("ID:%X\n", VIN_data.can_id);
    printf("DLC:%X\n", VIN_data.can_dlc);
    for(int i = 0; i < VIN_data.can_dlc; i ++) {
        printf("%X ", VIN_data.data[i]);
    }
    printf("\n");
    printf("Create first frame\n");
    VIN_responce.can_id = 0x7E8;
    VIN_responce.can_dlc = 8;
    VIN_responce.data[0] = 0x10;
    VIN_responce.data[1] = 0x14;
    VIN_responce.data[2] = 0x62;
    VIN_responce.data[3] = 0xF1;
    VIN_responce.data[4] = 0x90;
    for(int i = 5, j = 0; i < VIN_responce.can_dlc; i ++, j ++) {
        VIN_responce.data[i] = vin[j];
    }
    usleep(WAIT_TIME);
    printf("Send first frame\n");
    write(can_socket, &VIN_responce, sizeof(VIN_responce));
    printf("Send first frame success!\n");

    printf("Receive data\n");
    read(can_socket, &VIN_data, sizeof(VIN_data));
    usleep(WAIT_TIME);
    printf("ID:%X\n", VIN_data.can_id);
    printf("DLC:%X\n", VIN_data.can_dlc);
    for(int i = 0; i < VIN_data.can_dlc; i ++) {
        printf("%X ", VIN_data.data[i]);
    }
    printf("\n");

    uint8_t data_head = 0x21;
    int j = 3;
    int n = 0;

    printf("Create consecutive frame\n");
    while(n < 2) {
        VIN_responce.can_id = 0x7E8;
        VIN_responce.can_dlc = 8;
        VIN_responce.data[0] = data_head;
        for(int i = 1; i < VIN_responce.can_dlc; i++) {
            VIN_responce.data[i] = vin[j];
            j++;
        }
        printf("Send consecutive frame\n");
        usleep(WAIT_TIME);
        write(can_socket, &VIN_responce, sizeof(VIN_responce));
        printf("Send consecutive frame success!\n");
        data_head++;
        n ++;
    }
    return 0;
}