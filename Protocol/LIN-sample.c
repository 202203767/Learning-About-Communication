#include <stdio.h>
#include <stdint.h>

#define MAX_DATA 8
#define LIN_SYNC 0x55
#define BREAK_FIELD 0x00
#define PID_SPEED 0x01

//LINフレーム
typedef struct LIN {
    uint8_t break_field;
    uint8_t sync_byte_field;
    uint8_t protected_indefinder_field;
    uint8_t data[MAX_DATA];
    uint8_t checksum;
} LIN;

int main(void) {
    LIN sample;
    sample.break_field = BREAK_FIELD;
    sample.sync_byte_field = LIN_SYNC;
    sample.protected_indefinder_field = PID_SPEED;
    
    return 0;
}