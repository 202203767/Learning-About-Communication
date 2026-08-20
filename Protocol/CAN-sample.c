#include <stdio.h>
#include <stdint.h>

#define MAX_DATA_FIELD 8

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
    uint8_t Data[MAX_DATA_FIELD]; //0~64bit
    CRC_Field CRC; //16bit
    ACK_Field ACK; //2bit
    uint8_t eof; //7bit
} CAN;

void print_CAN(CAN CAN) {
    printf("%d\n", CAN.SOF);
    printf("%d\n", CAN.Arbitration.Base_ID);
    printf("%d\n", CAN.Arbitration.RTR);
    printf("%d\n", CAN.Control.IDE);
    printf("%d\n", CAN.Control.FDF);
    printf("%d\n", CAN.Control.DLC);
    for(int i = 0; i < CAN.Control.DLC; i ++) {
        printf("%d\n", CAN.Data[i]);
    }
    printf("%d\n", CAN.ACK.ACK_Slots);
    printf("%d\n", CAN.ACK.ACK_Slots);
    printf("%d\n", CAN.eof);
}

void mask(CAN mask_CAN) {
    mask_CAN.SOF &= 0x01;
    mask_CAN.Arbitration.Base_ID &= 0x7FF;
    mask_CAN.Arbitration.RTR &= 0x01;
    mask_CAN.Control.IDE &= 0x01;
    mask_CAN.Control.FDF = 0x01;
    mask_CAN.Control.DLC &= 0x0F;
    mask_CAN.ACK.ACK_Slots &= 0x01;
    mask_CAN.ACK.ACK_Delimiter &= 0x01;
    mask_CAN.eof &= 0x7F;
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

    mask(sample_CAN); //ビット数に合わせてマスク
    print_CAN(sample_CAN);

    return 0;
}