#include <stdio.h>
#include <stdint.h>

#define MAX_DATA_FIELD 8
#define CAN_Expansion_MAX_SIZE 29

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
} CAN_Expansion;

void print_CAN(CAN_Expansion CAN_Expansion) {
    printf("Start of frame:%d\n", CAN_Expansion.SOF);
    printf("Arbitration field Base ID:%d\n", CAN_Expansion.Arbitration.Base_ID);
    printf("Arbitration field SRR:%d\n", CAN_Expansion.Arbitration.SRR);
    printf("Arbitration field IDE:%d\n", CAN_Expansion.Arbitration.IDE);
    printf("Arbitration field ID Extension:%d\n", CAN_Expansion.Arbitration.ID_Extension);
    printf("Arbitration field RTR:%d\n", CAN_Expansion.Arbitration.RTR);
    printf("Control field FDF:%d\n", CAN_Expansion.Control.FDF);
    printf("Control field r0:%d\n", CAN_Expansion.Control.r0);
    printf("Control field DLC:%d\n", CAN_Expansion.Control.DLC);
    for(int i = 0; i < CAN_Expansion.Control.DLC; i ++) {
        printf("Data field:%d\n", CAN_Expansion.Data[i]);
    }
    printf("ACK field Slots:%d\n", CAN_Expansion.ACK.ACK_Slots);
    printf("ACK field Delimiter:%d\n", CAN_Expansion.ACK.ACK_Delimiter);
    printf("End for frame:%d\n", CAN_Expansion.eof);
}

void mask(CAN_Expansion *mask_CAN_Expansion) {
    mask_CAN_Expansion->SOF &= 0x01;
    mask_CAN_Expansion->Arbitration.Base_ID &= 0x7FF;
    mask_CAN_Expansion->Arbitration.SRR &= 0x01;
    mask_CAN_Expansion->Arbitration.IDE &= 0x01;
    mask_CAN_Expansion->Arbitration.ID_Extension &= 0x3FFFF;
    mask_CAN_Expansion->Arbitration.RTR &= 0x01;
    mask_CAN_Expansion->Control.FDF &= 0x01;
    mask_CAN_Expansion->Control.r0 &= 0x01;
    mask_CAN_Expansion->Control.DLC &= 0x0F;
    mask_CAN_Expansion->CRC.CRC &= 0x7FFF;
    mask_CAN_Expansion->CRC.CRC_Delimiter &= 0x01;
    mask_CAN_Expansion->ACK.ACK_Slots &= 0x01;
    mask_CAN_Expansion->ACK.ACK_Delimiter &= 0x01;
    mask_CAN_Expansion->eof &= 0x7F;
}

void send_and_receive(uint8_t send[]) {
    CAN_Expansion receive_CAN_Expansion;
    receive_CAN_Expansion.SOF = send[0];
    receive_CAN_Expansion.Arbitration.Base_ID = (send[1] << 8) | send[2];
    receive_CAN_Expansion.Arbitration.SRR = send[3];
    receive_CAN_Expansion.Arbitration.IDE = send[4];
    receive_CAN_Expansion.Arbitration.ID_Extension = (send[5] << 16) | (send[6] << 8) | send[7];
    receive_CAN_Expansion.Arbitration.RTR = send[8];
    receive_CAN_Expansion.Control.FDF = send[9];
    receive_CAN_Expansion.Control.r0 = send[10];
    receive_CAN_Expansion.Control.DLC = send[11];
    for(int i = 0; i < receive_CAN_Expansion.Control.DLC; i ++) {
        receive_CAN_Expansion.Data[i] = send[12 + i];
    }
    receive_CAN_Expansion.CRC.CRC = (send[12 + receive_CAN_Expansion.Control.DLC] << 8) | send[13 + receive_CAN_Expansion.Control.DLC];
    receive_CAN_Expansion.CRC.CRC_Delimiter = send[14 + receive_CAN_Expansion.Control.DLC];
    receive_CAN_Expansion.ACK.ACK_Slots = send[15 + receive_CAN_Expansion.Control.DLC];
    receive_CAN_Expansion.ACK.ACK_Delimiter = send[16 + receive_CAN_Expansion.Control.DLC];
    receive_CAN_Expansion.eof = send[17 + receive_CAN_Expansion.Control.DLC];
    printf("receive\n");
    print_CAN(receive_CAN_Expansion);
}

int main(void) {
    CAN_Expansion sample_CAN_expansion;
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

    uint8_t send[CAN_Expansion_MAX_SIZE];
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