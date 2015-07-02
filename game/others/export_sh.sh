#!/usr/bin/env bash
for i in 1 2 3 4 5 6 7 8
do 
  export "PLAYER"$i"_IP"=192.168.159.11
  export "PLAYER"$i"_PORT"=600$i
  export "PLAYER"$i"_ID"=$i$i$i$i
done
export 
./gameserver -gip 192.168.159.11 -seq replay -d 1 -m 10000 -b 50 -t 2000 -h 500