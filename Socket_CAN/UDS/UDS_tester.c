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

typedef struct {
    uint8_t sid;
    uint8_t did_high;
    uint8_t did_low;
    uint8_t data_high;
    uint8_t data_low;
} UDS_reqest;

int main(void) {
    int num = 0;
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame UDS_send, UDS_receive;
    UDS_reqest Dinamic_session = {0x10, 0x10, 0x03};
    UDS_reqest ECU_reset = {0x11, 0x11, 0x01};
    UDS_reqest Read_data = {0x22, 0xF1, 0x90};
    UDS_reqest Write_data = {0x2E, 0xF1, 0x89, 0x12, 0x34};
    UDS_reqest Read_DTC = {0x19, 0x19, 0x02};

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

    UDS_send.can_dlc = 3;
    while(1) {
        UDS_send.can_id = 0x7E0;
        printf("Which send?\n1.Diagnostic Session Control \n2.ECU Reset 3.Read Data By Identifier \n4.Write Data By Identifier \n5.Read DTC Information\n:");
        scanf("%d", &num);
        switch(num) {
            case 1: {
                UDS_send.can_dlc = 3;
                UDS_send.data[0] = Dinamic_session.sid;
                UDS_send.data[1] = Dinamic_session.did_high;
                UDS_send.data[2] = Dinamic_session.did_low;
                break;
            }
            case 2: {
                UDS_send.can_dlc = 3;
                UDS_send.data[0] = ECU_reset.sid;
                UDS_send.data[1] = ECU_reset.did_high;
                UDS_send.data[2] = ECU_reset.did_low;
                break;
            }
            case 3: {
                UDS_send.can_dlc = 3;
                UDS_send.data[0] = Read_data.sid;
                UDS_send.data[1] = Read_data.did_high;
                UDS_send.data[2] = Read_data.did_low;
                break;
            }
            case 4: {
                UDS_send.can_dlc = 5;
                UDS_send.data[0] = Write_data.sid;
                UDS_send.data[1] = Write_data.did_high;
                UDS_send.data[2] = Write_data.did_low;
                UDS_send.data[3] = Write_data.data_high;
                UDS_send.data[4] = Write_data.data_low;
                break;
            }
            case 5: {
                UDS_send.can_dlc = 3;
                UDS_send.data[0] = Read_DTC.sid;
                UDS_send.data[1] = Read_DTC.did_high;
                UDS_send.data[2] = Read_DTC.did_low;
                break;
            }
            default: {
                return 0;
            }
        }

        printf("send data\n");
        usleep(WAIT_TIME);
        write(can_socket, &UDS_send, sizeof(UDS_send));
        printf("send data is success!\n");

        read(can_socket, &UDS_receive, sizeof(UDS_receive));
        printf("receive data\n");
        usleep(WAIT_TIME);
        printf("ID:%X\n", UDS_receive.can_id);
        printf("DLC:%d\n", UDS_receive.can_dlc);
        for(int i = 0; i < UDS_receive.can_dlc; i ++) {
            printf("%X \n", UDS_receive.data[i]);
        }
        printf("\n");
        usleep(WAIT_TIME);
    }
    return 0;
}