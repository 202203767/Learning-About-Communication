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

#define SPEED 0x100
#define OIL_TEMP 0x101
#define WATER_TEMP 0x102
#define RPM 0x103
#define ID_TYPES 3
#define MAX_SOCKET 1
#define WAIT_TIME 2000000 //マイクロ単位

