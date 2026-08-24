#include <stdio.h>
#include <stdint.h>

#define MAX_DATA_FIELD 64
#define CAN_FD_MAX_SIZE 20

typedef struct Arbitration_Field {
    uint16_t Base_ID; //11bit
    uint8_t RRS; //1bit
} Arbitration_Field;

typedef struct Control_Field {
    uint8_t IDE; //1bit
    uint8_t FDF; //1bit
    uint8_t BRS; //1bit
    uint8_t ESI; //1bit
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
} CAN_FD;

void print_CAN(CAN_FD CAN_FD) {
    printf("Start of frame:%d\n", CAN_FD.SOF);
    printf("Arbitration field Base ID:%d\n", CAN_FD.Arbitration.Base_ID);
    printf("Arbitration field RTR:%d\n", CAN_FD.Arbitration.RRS);
    printf("Control field IDE:%d\n", CAN_FD.Control.IDE);
    printf("Control field FDF:%d\n", CAN_FD.Control.FDF);
    printf("Control field BRS:%d\n", CAN_FD.Control.BRS);
    printf("Control field ESI:%d\n", CAN_FD.Control.ESI);
    printf("Control field DLC:%d\n", CAN_FD.Control.DLC);
    for(int i = 0; i < CAN_FD.Control.DLC; i ++) {
        printf("Data field:%d\n", CAN_FD.Data[i]);
    }
    printf("ACK field Slots:%d\n", CAN_FD.ACK.ACK_Slots);
    printf("ACK field Delimiter:%d\n", CAN_FD.ACK.ACK_Delimiter);
    printf("End for frame:%d\n", CAN_FD.eof);
}

void mask(CAN_FD *mask_CAN_FD) {
    mask_CAN_FD->SOF &= 0x01;
    mask_CAN_FD->Arbitration.Base_ID &= 0x7FF;
    mask_CAN_FD->Arbitration.RRS &= 0x01;
    mask_CAN_FD->Control.IDE &= 0x01;
    mask_CAN_FD->Control.FDF &= 0x01;
    mask_CAN_FD->Control.BRS &= 0x01;
    mask_CAN_FD->Control.ESI &= 0x01;
    mask_CAN_FD->Control.DLC &= 0x0F;
    mask_CAN_FD->CRC.CRC &= 0x7FFF;
    mask_CAN_FD->CRC.CRC_Delimiter &= 0x01;
    mask_CAN_FD->ACK.ACK_Slots &= 0x01;
    mask_CAN_FD->ACK.ACK_Delimiter &= 0x01;
    mask_CAN_FD->eof &= 0x7F;
}

void send_and_receive(uint8_t send[]) {
    CAN_FD receive_CAN_FD;
    receive_CAN_FD.SOF = send[0];
    receive_CAN_FD.Arbitration.Base_ID = (send[1] << 8) | send[2];
    receive_CAN_FD.Arbitration.RRS = send[3];
    receive_CAN_FD.Control.IDE = send[4];
    receive_CAN_FD.Control.FDF = send[5];
    receive_CAN_FD.Control.BRS = send[6];
    receive_CAN_FD.Control.ESI = send[7];
    receive_CAN_FD.Control.DLC = send[8];
    for(int i = 0; i < receive_CAN_FD.Control.DLC; i ++) {
        receive_CAN_FD.Data[i] = send[9 + i];
    }
    receive_CAN_FD.CRC.CRC = (send[9 + receive_CAN_FD.Control.DLC] << 8) | send[10 + receive_CAN_FD.Control.DLC];
    receive_CAN_FD.CRC.CRC_Delimiter = send[11 + receive_CAN_FD.Control.DLC];
    receive_CAN_FD.ACK.ACK_Slots = send[12 + receive_CAN_FD.Control.DLC];
    receive_CAN_FD.ACK.ACK_Delimiter = send[13 + receive_CAN_FD.Control.DLC];
    receive_CAN_FD.eof = send[14 + receive_CAN_FD.Control.DLC];
    printf("receive\n");
    print_CAN(receive_CAN_FD);
}

int main(void) {
    CAN_FD sample_CAN_FD;
    sample_CAN_FD.SOF = 0;
    sample_CAN_FD.Arbitration.Base_ID = 0x100;
    sample_CAN_FD.Arbitration.RRS = 0;
    sample_CAN_FD.Control.IDE = 0;
    sample_CAN_FD.Control.FDF = 0;
    sample_CAN_FD.Control.BRS = 1;
    sample_CAN_FD.Control.ESI = 0;
    sample_CAN_FD.Control.DLC = 2;
    sample_CAN_FD.Data[0] = 0x05;
    sample_CAN_FD.Data[1] = 0x62;
    sample_CAN_FD.CRC.CRC = 0;
    sample_CAN_FD.CRC.CRC_Delimiter = 1;
    sample_CAN_FD.ACK.ACK_Slots = 1;
    sample_CAN_FD.ACK.ACK_Delimiter = 1;
    sample_CAN_FD.eof = 0x7F;

    mask(&sample_CAN_FD); //ビット数に合わせてマスク
    printf("send\n");
    print_CAN(sample_CAN_FD);

    uint8_t send[CAN_FD_MAX_SIZE];
    send[0] = sample_CAN_FD.SOF;
    send[1] = (sample_CAN_FD.Arbitration.Base_ID >> 8) & 0xFF;
    send[2] = sample_CAN_FD.Arbitration.Base_ID & 0xFF;
    send[3] = sample_CAN_FD.Arbitration.RRS;
    send[4] = sample_CAN_FD.Control.IDE;
    send[5] = sample_CAN_FD.Control.FDF;
    send[6] = sample_CAN_FD.Control.BRS;
    send[7] = sample_CAN_FD.Control.ESI;
    send[8] = sample_CAN_FD.Control.DLC;
    send[9] = sample_CAN_FD.Data[0];
    send[10] = sample_CAN_FD.Data[1];
    send[11] = (sample_CAN_FD.CRC.CRC >> 8) & 0xFF;
    send[12] = sample_CAN_FD.CRC.CRC & 0xFF;
    send[13] = sample_CAN_FD.CRC.CRC_Delimiter;
    send[14] = sample_CAN_FD.ACK.ACK_Slots;
    send[15] = sample_CAN_FD.ACK.ACK_Delimiter;
    send[16] = sample_CAN_FD.eof;

    send_and_receive(send);//送信（仮定）
    return 0;
}