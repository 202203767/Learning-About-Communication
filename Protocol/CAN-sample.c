#include <stdio.h>
#include <stdint.h>

#define MAX_DATA_FIELD 8
#define CAN_MAX_SIZE 15

typedef struct Arbitration_Field {
    uint16_t Base_ID; //11bit
    uint8_t RTR; //1bit
} Arbitration_Field;

typedef struct Control_Field {
    uint8_t IDE; //1bit
    uint8_t FDF; //1bit
    uint8_t DLC; //4bit
} Control_Field;

typedef struct CRC_Field {
    uint16_t CRC; //15bit
    uint8_t CRC_Delimiter; //1bit
} CRC_Field;

typedef struct ACK_Field {
    uint8_t ACK_Slots; //1bit
    uint8_t ACK_Delimiter; //1bit
} ACK_Field;

typedef struct CAN {
    uint8_t SOF; //1bit
    Arbitration_Field Arbitration; //12bit
    Control_Field Control; //6bit
    uint8_t Data[MAX_DATA_FIELD]; //0~8byte
    CRC_Field CRC; //16bit
    ACK_Field ACK; //2bit
    uint8_t eof; //7bit
} CAN;

void print_CAN(CAN CAN) {
    printf("Start of frame:%d\n", CAN.SOF);
    printf("Arbitration field Base ID:%d\n", CAN.Arbitration.Base_ID);
    printf("Arbitration field RTR:%d\n", CAN.Arbitration.RTR);
    printf("Control field IDE:%d\n", CAN.Control.IDE);
    printf("Control field FDF:%d\n", CAN.Control.FDF);
    printf("Control field DLC:%d\n", CAN.Control.DLC);
    for(int i = 0; i < CAN.Control.DLC; i ++) {
        printf("Data field:%d\n", CAN.Data[i]);
    }
    printf("ACK field Slots:%d\n", CAN.ACK.ACK_Slots);
    printf("ACK field Delimiter:%d\n", CAN.ACK.ACK_Delimiter);
    printf("End for frame:%d\n", CAN.eof);
}

void mask(CAN *mask_CAN) {
    mask_CAN->SOF &= 0x01;
    mask_CAN->Arbitration.Base_ID &= 0x7FF;
    mask_CAN->Arbitration.RTR &= 0x01;
    mask_CAN->Control.IDE &= 0x01;
    mask_CAN->Control.FDF &= 0x01;
    mask_CAN->Control.DLC &= 0x0F;
    mask_CAN->CRC.CRC &= 0x7FFF;
    mask_CAN->CRC.CRC_Delimiter &= 0x01;
    mask_CAN->ACK.ACK_Slots &= 0x01;
    mask_CAN->ACK.ACK_Delimiter &= 0x01;
    mask_CAN->eof &= 0x7F;
}

void send_and_receive(uint8_t send[]) {
    CAN receive_CAN;
    receive_CAN.SOF = send[0];
    receive_CAN.Arbitration.Base_ID = (send[1] << 8) | send[2];
    receive_CAN.Arbitration.RTR = send[3];
    receive_CAN.Control.IDE = send[4];
    receive_CAN.Control.FDF = send[5];
    receive_CAN.Control.DLC = send[6];
    for(int i = 0; i < receive_CAN.Control.DLC; i ++) {
        receive_CAN.Data[i] = send[7 + i];
    }
    receive_CAN.CRC.CRC = (send[7 + receive_CAN.Control.DLC] << 8) | send[8 + receive_CAN.Control.DLC];
    receive_CAN.CRC.CRC_Delimiter = send[9 + receive_CAN.Control.DLC];
    receive_CAN.ACK.ACK_Slots = send[10 + receive_CAN.Control.DLC];
    receive_CAN.ACK.ACK_Delimiter = send[11 + receive_CAN.Control.DLC];
    receive_CAN.eof = send[12 + receive_CAN.Control.DLC];
    printf("receive\n");
    print_CAN(receive_CAN);
}

int main(void) {
    CAN sample_CAN;
    sample_CAN.SOF = 0;
    sample_CAN.Arbitration.Base_ID = 0x100;
    sample_CAN.Arbitration.RTR = 0;
    sample_CAN.Control.IDE = 0;
    sample_CAN.Control.FDF = 0;
    sample_CAN.Control.DLC = 2;
    sample_CAN.Data[0] = 0x05;
    sample_CAN.Data[1] = 0x62;
    sample_CAN.CRC.CRC = 0;
    sample_CAN.CRC.CRC_Delimiter = 1;
    sample_CAN.ACK.ACK_Slots = 1;
    sample_CAN.ACK.ACK_Delimiter = 1;
    sample_CAN.eof = 0x7F;


    mask(&sample_CAN); //ビット数に合わせてマスク
    printf("send\n");
    print_CAN(sample_CAN);

    uint8_t send[CAN_MAX_SIZE];
    send[0] = sample_CAN.SOF;
    send[1] = (sample_CAN.Arbitration.Base_ID >> 8) & 0xFF;
    send[2] = sample_CAN.Arbitration.Base_ID & 0xFF;
    send[3] = sample_CAN.Arbitration.RTR;
    send[4] = sample_CAN.Control.IDE;
    send[5] = sample_CAN.Control.FDF;
    send[6] = sample_CAN.Control.DLC;
    send[7] = sample_CAN.Data[0];
    send[8] = sample_CAN.Data[1];
    send[9] = (sample_CAN.CRC.CRC >> 8) & 0xFF;
    send[10] = sample_CAN.CRC.CRC & 0xFF;
    send[11] = sample_CAN.CRC.CRC_Delimiter;
    send[12] = sample_CAN.ACK.ACK_Slots;
    send[13] = sample_CAN.ACK.ACK_Delimiter;
    send[14] = sample_CAN.eof;

    send_and_receive(send);//送信（仮定）
    return 0;
}