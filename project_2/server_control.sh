#!/bin/bash

echo "-----------------------------------------------------------------------------"
echo " Server Control "
echo "-----------------------------------------------------------------------------"
echo " 1 - Launch Servers"
echo " 2 - Kill All Active Servers"
echo ""
echo -n "Please enter number: "
read ACTION
echo "-----------------------------------------------------------------------------"

case $ACTION in

  1)
    # this must match the IPs in config.txt!!!!
    ./server.out 127.0.0.1 5555 &
    ./server.out 127.0.0.1 5556 &
    ./server.out 127.0.0.1 5557 &
    ./server.out 127.0.0.1 5558 &
    ./server.out 127.0.0.1 5559 &

    ;;

  2)
    killall server.out
    ;;

  *)
    echo -n "Unrecognized action"
    ;;
esac