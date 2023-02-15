#!/bin/bash

echo "-----------------------------------------------------------------------------"
echo " Available Test Cases"
echo "-----------------------------------------------------------------------------"
echo " 1 - Start client with server down - confirm server connection error"
echo " 2 - Start client with server up, and then kill server - confirm client detects server timeout"
echo " 3 - Start client with server up, and then kill client - confirm that server detects client timeout"
echo " 4 - Try to add too many clients - confirm server prints error messages"
echo " 5 - Have two clients connect, one subscribes, one published - confirm that subscriber gets message"
echo " 6 - Have three clients connect, two subscribe, one published - confirm that both subscriber gets message"
echo " 7 - Have two clients connect, one subscriber, one published, and then subscriber unsubscribes - confirm that subscriber does not receive message since it has unsubscribed"
echo ""
echo -n "Please enter test number: "
read TEST_CASE
echo "-----------------------------------------------------------------------------"

case $TEST_CASE in

  1)
    ./client.out 127.0.0.1

    ;;

  2)
    ./server.out &
    sleep 1
 
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 &
    sleep 1

    kill %1 # kill server
    sleep 5

    pkill -P $$ # cleanup - kill all child processes
    
    ;;

  3)
    ./server.out &
    sleep 1
 
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 &
    sleep 1

    kill %2 # kill client
    sleep 20

    pkill -P $$ # cleanup - kill all child processes
    
    ;;

  4)
    ./server.out &
    sleep 1
 
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & # subscriber 1
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & # subscriber 2
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & # subscriber 3
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & # subscriber 4
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & # subscriber 5 !!!! one too many
    sleep 1

    pkill -P $$ # cleanup - kill all child processes
    
    ;;

  5)
    ./server.out &
    sleep 1
 
    ./client.out 127.0.0.1 "Sports;Tom;ESPN;" 20 & # subscriber
    sleep 1

    ./client.out 127.0.0.1 "Sports;Tom;ESPN;Hello this is Tom from ESPN!" & # publisher
    sleep 1

    pkill -P $$ # cleanup - kill all child processes
    
    ;;

  *)
    echo -n "Unrecognized test case"
    ;;
esac