#!/usr/bin/env bash

echo "start playmates"
./random   127.0.0.1 6000 127.0.0.1 6001 1111 0</dev/null 1>/dev/null 2>/dev/null &
./call     127.0.0.1 6000 127.0.0.2 6002 2222 0</dev/null 1>/dev/null 2>/dev/null &
./check    127.0.0.1 6000 127.0.0.3 6003 3333 0</dev/null 1>/dev/null 2>/dev/null &
./fold     127.0.0.1 6000 127.0.0.4 6004 4444 0</dev/null 1>/dev/null 2>/dev/null &
./raise1   127.0.0.1 6000 127.0.0.5 6005 5555 0</dev/null 1>/dev/null 2>/dev/null &
./raise100 127.0.0.1 6000 127.0.0.6 6006 6666 0</dev/null 1>/dev/null 2>/dev/null &
./random   127.0.0.1 6000 127.0.0.7 6007 7777 0</dev/null 1>/dev/null 2>/dev/null &

echo "start your game"
./game 127.0.0.1 6000 127.0.0.8 6008 8888