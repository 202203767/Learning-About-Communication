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
#include <sys/select.h>

#define WAIT_TIME 2000000 //マイクロ単位
#define MAX_SOCKET 1

typedef struct {
    uint8_t sid;
    uint8_t did_high;
    uint8_t did_low;

    uint8_t data_high;
    uint8_t data_low;
} UDS_receive;

typedef struct{
    uint16_t did;
    uint16_t value;
} DID_Table;


int main(void) {
    uint8_t session = 0x01;
    uint8_t unlock = 0;
    int can_socket = 0;
    char interface[IFNAMSIZ] = "vcan0";
    struct ifreq network_setup;//警告が出るが正常に動くので無視
    struct sockaddr_can addr;
    struct can_frame UDS_responce, UDS_data;

    UDS_receive ECU_reset = {0x51, 0x01};
    UDS_receive Read_data = {0x62, 0xF1, 0x90, 0x12, 0x34};
    UDS_receive Write_data = {0x6E, 0xF1, 0x89};
    UDS_receive Read_DTC = {0x59, 0x02, 0x12, 0x34, 0x56};
    DID_Table table[] = {{0xF190, 0x1234}, {0xF189, 0x5678}, {0xF187, 0xABCD}};

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
        fd_set readfds;
        struct timeval timeout;
    
        FD_ZERO(&readfds);
        FD_SET(can_socket, &readfds);
        timeout.tv_sec = 20;
        timeout.tv_usec = 0;
        int ret = select(can_socket + 1,&readfds,NULL,NULL,&timeout);
        if(ret == 0) {
            printf("Time Out\n");
            break;
        } else if(ret < 0) {
            perror("select");
            break;
        }

        if(FD_ISSET(can_socket, &readfds)) {
            read(can_socket, &UDS_data, sizeof(UDS_data));
            printf("receive data success!");
            printf("Current Session : 0x%02X\n",session);
            usleep(WAIT_TIME);
            printf("ID:%X\n", UDS_data.can_id);
            printf("DLC:%X\n", UDS_data.can_dlc);
            for(int i = 0; i < UDS_data.can_dlc; i ++) {
                printf("%X ", UDS_data.data[i]);
            }
            printf("\n");
            UDS_responce.can_id = 0x7E8;
            switch(UDS_data.data[0]) {
                case 0x10: {
                    printf("Diagnostic Session Control\n");
                    session = UDS_data.data[1];
                    UDS_responce.can_dlc = 2;
                    UDS_responce.data[0] = 0x50;
                    UDS_responce.data[1] = session;
                    break;
                }
                case 0x11: {
                    printf("ECU Reset\n");
                    UDS_responce.can_dlc = 2;
                    UDS_responce.data[0] = ECU_reset.sid;
                    UDS_responce.data[1] = ECU_reset.did_high;
                    break;
                }
                case 0x22: {
                    printf("Read Data By Identifier\n");
                    UDS_responce.can_dlc = 5;
                    UDS_responce.data[0] = Read_data.sid;
                    UDS_responce.data[1] = Read_data.did_high;
                    UDS_responce.data[2] = Read_data.did_low;
                    UDS_responce.data[3] = Read_data.data_high;
                    UDS_responce.data[4] = Read_data.data_low;
                    break;
                }
                case 0x2E: {
                    printf("Write Data By Identifier\n");
                    UDS_responce.can_dlc = 3;
                    if(session != 0x03 || unlock == 0) {
                        UDS_responce.data[0] = 0x7F;
                        UDS_responce.data[1] = 0x2E;
                        UDS_responce.data[2] = 0x22;
                        break;
                    }
                    UDS_responce.data[0] = Write_data.sid;
                    UDS_responce.data[1] = Write_data.did_high;
                    UDS_responce.data[2] = Write_data.did_low;
                    break;
                }
                case 0x19: {
                    printf("Read DTC Information\n");
                    UDS_responce.can_dlc = 5;
                    UDS_responce.data[0] = Read_DTC.sid;
                    UDS_responce.data[1] = Read_DTC.did_high;
                    UDS_responce.data[2] = Read_DTC.did_low;
                    UDS_responce.data[3] = Read_DTC.data_high;
                    UDS_responce.data[4] = Read_DTC.data_low;
                    break;
                }
                case 0x27: {
                    printf("Security Access\n");
                    if(UDS_data.data[1] == 0x01) {
                        UDS_responce.can_dlc = 4;
                        UDS_responce.data[0] = 0x67;
                        UDS_responce.data[1] = 0x01;
                        UDS_responce.data[2] = 0x12;
                        UDS_responce.data[3] = 0x34;
                    } else if(UDS_data.data[1] == 0x02) {
                        if(UDS_data.data[2] = 0x12 && UDS_data.data[3] == 0x34) {
                            unlock = 1;

                            UDS_responce.can_dlc = 2;
                            UDS_responce.data[0] = 0x67;
                            UDS_responce.data[1] = 0x02;
                        }
                    }
                    break;
                }
                default: {
                    UDS_responce.can_dlc = 3;
                    UDS_responce.data[0] = 0x7F;
                    UDS_responce.data[1] = UDS_data.data[0];
                    UDS_responce.data[2] = 0x11;
                    break;
                }
            }
            printf("send data\n");
            usleep(WAIT_TIME);
            write(can_socket, &UDS_responce, sizeof(UDS_responce));
            printf("send data is success!\n");
        }
    }
    return 0;
}