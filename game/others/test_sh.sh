#!/usr/bin/env bash

for i in 1 2 3 4 5 6 7
do
./game 127.0.0.1 6000 127.0.0.$i 600$i $i$i$i$i 0</dev/null 1>/dev/null 2>/dev/null &
done

./game 127.0.0.1 6000 127.0.0.8 6008 8888