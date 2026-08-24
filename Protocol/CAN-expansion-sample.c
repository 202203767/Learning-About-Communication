#include <stdio.h>
#include <stdint.h>

#define MAX_DATA_FIELD 8
#define CAN_MAX_SIZE 29

typedef struct Arbitration_Field {
    uint16_t Base_ID; //11bit
    uint8_t SRR; //1bit
    uint8_t IDE; //1bit
    uint32_t ID_Extension; //18bit
    uint8_t RTR; //1bit
} Arbitration_Field;

typedef struct Control_Field {
    uint8_t FDF; //1bit
    uint8_t r0; //1bit
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
    printf("Arbitration field SRR:%d\n", CAN.Arbitration.SRR);
    printf("Arbitration field IDE:%d\n", CAN.Arbitration.IDE);
    printf("Arbitration field ID Extension:%d\n", CAN.Arbitration.ID_Extension);
    printf("Arbitration field RTR:%d\n", CAN.Arbitration.RTR);
    printf("Control field FDF:%d\n", CAN.Control.FDF);
    printf("Control field r0:%d\n", CAN.Control.r0);
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
    mask_CAN->Arbitration.SRR &= 0x01;
    mask_CAN->Arbitration.IDE &= 0x01;
    mask_CAN->Arbitration.ID_Extension &= 0x3FFFF;
    mask_CAN->Arbitration.RTR &= 0x01;
    mask_CAN->Control.FDF &= 0x01;
    mask_CAN->Control.r0 &= 0x01;
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
    receive_CAN.Arbitration.SRR = send[3];
    receive_CAN.Arbitration.IDE = send[4];
    receive_CAN.Arbitration.ID_Extension = (send[5] << 16) | (send[6] << 8) | send[7];
    receive_CAN.Arbitration.RTR = send[8];
    receive_CAN.Control.FDF = send[9];
    receive_CAN.Control.r0 = send[10];
    receive_CAN.Control.DLC = send[11];
    for(int i = 0; i < receive_CAN.Control.DLC; i ++) {
        receive_CAN.Data[i] = send[12 + i];
    }
    receive_CAN.CRC.CRC = (send[12 + receive_CAN.Control.DLC] << 8) | send[13 + receive_CAN.Control.DLC];
    receive_CAN.CRC.CRC_Delimiter = send[14 + receive_CAN.Control.DLC];
    receive_CAN.ACK.ACK_Slots = send[15 + receive_CAN.Control.DLC];
    receive_CAN.ACK.ACK_Delimiter = send[16 + receive_CAN.Control.DLC];
    receive_CAN.eof = send[17 + receive_CAN.Control.DLC];
    printf("receive\n");
    print_CAN(receive_CAN);
}

int main(void) {
    CAN sample_CAN_expansion;
    sample_CAN_expansion.SOF = 0;
    sample_CAN_expansion.Arbitration.Base_ID = 0x100;
    sample_CAN_expansion.Arbitration.SRR = 1;
    sample_CAN_expansion.Arbitration.IDE = 1;
    sample_CAN_expansion.Arbitration.ID_Extension = 0x12345;
    sample_CAN_expansion.Arbitration.RTR = 0;
    sample_CAN_expansion.Control.FDF = 0;
    sample_CAN_expansion.Control.r0 = 0;
    sample_CAN_expansion.Control.DLC = 2;
    sample_CAN_expansion.Data[0] = 0x05;
    sample_CAN_expansion.Data[1] = 0x62;
    sample_CAN_expansion.CRC.CRC = 0;
    sample_CAN_expansion.CRC.CRC_Delimiter = 1;
    sample_CAN_expansion.ACK.ACK_Slots = 1;
    sample_CAN_expansion.ACK.ACK_Delimiter = 1;
    sample_CAN_expansion.eof = 0x7F;


    mask(&sample_CAN_expansion); //ビット数に合わせてマスク
    printf("send\n");
    print_CAN(sample_CAN_expansion);

    uint8_t send[CAN_MAX_SIZE];
    send[0] = sample_CAN_expansion.SOF;
    send[1] = (sample_CAN_expansion.Arbitration.Base_ID >> 8) & 0xFF;
    send[2] = sample_CAN_expansion.Arbitration.Base_ID & 0xFF;
    send[3] = sample_CAN_expansion.Arbitration.SRR;
    send[4] = sample_CAN_expansion.Arbitration.IDE;
    send[5] = (sample_CAN_expansion.Arbitration.ID_Extension >> 16) & 0xFF;
    send[6] = (sample_CAN_expansion.Arbitration.ID_Extension >> 8) & 0xFF;
    send[7] = sample_CAN_expansion.Arbitration.ID_Extension & 0xFF;
    send[8] = sample_CAN_expansion.Arbitration.RTR;
    send[9] = sample_CAN_expansion.Control.FDF;
    send[10] = sample_CAN_expansion.Control.r0;
    send[11] = sample_CAN_expansion.Control.DLC;
    send[12] = sample_CAN_expansion.Data[0];
    send[13] = sample_CAN_expansion.Data[1];
    send[14] = (sample_CAN_expansion.CRC.CRC >> 8) & 0xFF;
    send[15] = sample_CAN_expansion.CRC.CRC & 0xFF;
    send[16] = sample_CAN_expansion.CRC.CRC_Delimiter;
    send[17] = sample_CAN_expansion.ACK.ACK_Slots;
    send[18] = sample_CAN_expansion.ACK.ACK_Delimiter;
    send[19] = sample_CAN_expansion.eof;

    send_and_receive(send);//送信（仮定）
    return 0;
}