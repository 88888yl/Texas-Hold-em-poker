#!/usr/bin/env bash

while [ "1" = "1" ]
do

/home/game/test_game/server/killall.sh
sleep 1
/home/game/test_game/server/export_sh.sh &
sleep 1
/home/game/test_game/works/target/test_sh.sh
sleep 60

done