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
echo -n "Please enter test number:"
read TEST_CASE
echo "-----------------------------------------------------------------------------"

case $TEST_CASE in

  1)
    echo "test"
    ;;

  2)
    echo "test"
    ;;

  *)
    echo -n "Unrecognized test case"
    ;;
esac