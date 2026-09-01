概要
ifreq構造体
struct ifreq {
    char ifr_name[IFNAMSIZ];  /* インターフェース名 (例: "eth0") */
    union {
        struct sockaddr ifr_addr;       /* IPアドレス */
        struct sockaddr ifr_dstaddr;    /* 送信先アドレス */
        struct sockaddr ifr_broadaddr;  /* ブロードキャストアドレス */
        struct sockaddr ifr_netmask;    /* ネットマスク */
        struct sockaddr ifr_hwaddr;     /* ハードウェア(MAC)アドレス */
        short ifr_flags;                /* インターフェースフラグ */
        int ifr_ifindex;                /* インターフェースインデックス */
        int ifr_metric;                 /* メトリック */
        int ifr_mtu;                    /* MTU */
        struct ifmap ifr_map;           /* ハードウェアマップ */
        char ifr_slave[IFNAMSIZ];       /* スレーブ名 */
        char ifr_newname[IFNAMSIZ];     /* 新しい名前 */
        char *ifr_data;                 /* 任意データ */
    };
};

sockaddr_can構造体の中身
struct sockaddr {
   sa_family_t sa_family; // アドレスファミリ (例: AF_INET)
   char sa_data[14]; // アドレスとポート情報
};

ソケットCAN
動作方法
$gcc socket_can.c
$./a.out
send start
send complete

別ターミナルを開く
$sudo modprobe vcan
$sudo ip link add dev vcan0 type vcan
$sudo ip link set up vcan0
$ip link show vcan0
$candump vcan0


DBCの実行方法

$source can_env/bin/activate
$cd 100_C_file/Send_and_receive/Socket_CAN
$cantools dump can.dbc
================================= Messages =================================

  ------------------------------------------------------------------------

  Name:           SPEED
  Id:             0x100
  Length:         2 bytes
  Cycle time:     - ms
  Senders:        ECU
  Layout:

                          Bit

             7   6   5   4   3   2   1   0
           +---+---+---+---+---+---+---+---+
     B   0 |------------------------------x|
     y     +---+---+---+---+---+---+---+---+
     t   1 |<------------------------------|
     e     +---+---+---+---+---+---+---+---+
             +-- VehicleSpeed

  Signal tree:

    -- {root}
       +-- VehicleSpeed

  ------------------------------------------------------------------------

  Name:           OIL_TEMP
  Id:             0x101
  Length:         1 bytes
  Cycle time:     - ms
  Senders:        ECU
  Layout:

                          Bit

             7   6   5   4   3   2   1   0
           +---+---+---+---+---+---+---+---+
     B   0 |<-----------------------------x|
     y     +---+---+---+---+---+---+---+---+
     t       +-- OilTemperature
     e

  Signal tree:

    -- {root}
       +-- OilTemperature

  ------------------------------------------------------------------------

  Name:           WATER_TEMP
  Id:             0x102
  Length:         1 bytes
  Cycle time:     - ms
  Senders:        ECU
  Layout:

                          Bit

             7   6   5   4   3   2   1   0
           +---+---+---+---+---+---+---+---+
     B   0 |<-----------------------------x|
     y     +---+---+---+---+---+---+---+---+
     t       +-- WaterTemperature
     e

  Signal tree:

    -- {root}
       +-- WaterTemperature

  ------------------------------------------------------------------------

  Name:           RPM
  Id:             0x103
  Length:         2 bytes
  Cycle time:     - ms
  Senders:        ECU
  Layout:

                          Bit

             7   6   5   4   3   2   1   0
           +---+---+---+---+---+---+---+---+
     B   0 |------------------------------x|
     y     +---+---+---+---+---+---+---+---+
     t   1 |<------------------------------|
     e     +---+---+---+---+---+---+---+---+
             +-- EngineRPM

  Signal tree:

    -- {root}
       +-- EngineRPM

  ------------------------------------------------------------------------
  //こうなっていればOK
python3 DBC_test.py