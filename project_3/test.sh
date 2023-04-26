#!/bin/bash
clear;

killall -s SIGKILL server.out > /dev/null
killall -s SIGKILL peer.out > /dev/null

# create directory
rm -rf ./testing/
mkdir ./testing/
mkdir ./testing/remote_peer_1/
mkdir ./testing/remote_peer_2/
mkdir ./testing/remote_peer_3/
mkdir ./testing/remote_peer_4/
mkdir ./testing/remote_peer_5/
mkdir ./testing/local_peer/

# populate remote peer directories with files
echo Hello, I originate from remote peer 1. Have a great day! > ./testing/remote_peer_1/hello_from_peer_1.txt
echo Hello, I originate from remote peer 2. Have a great day! > ./testing/remote_peer_2/hello_from_peer_2.txt
echo Hello, I originate from remote peer 3. Have a great day! > ./testing/remote_peer_3/hello_from_peer_3.txt
echo Hello, I originate from remote peer 4. Have a great day! > ./testing/remote_peer_4/hello_from_peer_4.txt
echo Hello, I originate from remote peer 5. Have a great day! > ./testing/remote_peer_5/hello_from_peer_5.txt
echo 'Hello, I am common to all remote peers!' > ./testing/remote_peer_1/common.txt
echo 'Hello, I am common to all remote peers!' > ./testing/remote_peer_2/common.txt
echo 'Hello, I am common to all remote peers!' > ./testing/remote_peer_3/common.txt
echo 'Hello, I am common to all remote peers!' > ./testing/remote_peer_4/common.txt
echo 'Hello, I am common to all remote peers!' > ./testing/remote_peer_5/common.txt

# spawn server
./server.out 127.0.0.1 8888 > /dev/null &

# spawn remote peers in background
./peer.out 127.0.0.1 ./testing/remote_peer_1/ 127.0.0.1 8888 < /dev/null > /dev/null &
./peer.out 127.0.0.1 ./testing/remote_peer_2/ 127.0.0.1 8888 < /dev/null > /dev/null &
./peer.out 127.0.0.1 ./testing/remote_peer_3/ 127.0.0.1 8888 < /dev/null > /dev/null &
./peer.out 127.0.0.1 ./testing/remote_peer_4/ 127.0.0.1 8888 < /dev/null > /dev/null &
./peer.out 127.0.0.1 ./testing/remote_peer_5/ 127.0.0.1 8888 < /dev/null > /dev/null &

# run local peer for user to interact with
./peer.out 127.0.0.1 ./testing/local_peer/ 127.0.0.1 8888

# kill background processess
pkill -P $$
