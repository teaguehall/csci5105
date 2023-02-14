#!/bin/bash

echo "-----------------------------------------------------------------------------"
echo " Available Test Cases"
echo "-----------------------------------------------------------------------------"
echo " 1 - Start client with server down - confirm server connection error"
echo " 2 - Start client with server up, and then kill server - confirm client detects server timeout"
echo " 3 - Start client with server up, and then kill client - confirm that server detects client timeout"
echo " 4 - Have two clients connect, one subscribes, one published - confirm that subscriber gets message"
echo " 5 - Have three clients connect, two subscribe, one published - confirm that both subscriber gets message"
echo " 6 - Have two clients connect, one subscriber, one published, and then subscriber unsubscribes - confirm that subscriber does not receive message since it has unsubscribed "
echo ""
echo -n "Please enter test number: "
read TEST_CASE
echo "-----------------------------------------------------------------------------"
echo ""

case $TEST_CASE in

  1)
    ./client.out 127.0.0.1 

    pkill -P $$ # kill all child processes

    ;;

  2)
    ./server.out &
    sleep 1

    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & 
    sleep 1
    kill %1
    sleep 10

    pkill -P $$ # kill all child processes

    ;;    

  3)
    ./server.out &
    sleep 1

    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & 
    sleep 1
    kill %2
    sleep 10

    pkill -P $$ # kill all child processes

    ;;

  4)
    ./server.out &
    sleep 1

    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & # subscriber
    sleep 1

    ./client.out 127.0.0.1 "Sports;Tom;ESPN;Hello I'm Tom from ESPN" &  # publisher

    pkill -P $$ # kill all child processes

    ;;


  *)
    echo "Invalid test number provided"
    echo ""
    ;;
esac




