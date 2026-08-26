概要

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