概要

本プログラムは通信を学習するために実装したプログラムである。
初歩的なところで送信側のプログラムに自由にテキストを書いてもらって
受信側で受け取ったテキストを読むようにしている。

使い方
gcc send.c ../../input_char/input_char.c
./a.out
input messege:任意のメッセージ
send message!

gcc receive.c
./a.out