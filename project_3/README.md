QUICK START: 
- Run "make" on top-level Makefile to compile peer and server executables.
- Run "test.sh" script to launch a test environment (5 remote peers, 1 local peer for user-inputs)

CUSTOM START:
- Run "make" on top-level Makefile to compile peer and server executables.
- Start server via "./server.out <listening address> <listening port>"
- Start peer via "./peer.out <binding interface> <path-to-shared-folder> <server address> <server port>"

OVERVIEW: Source code for the P2P network is split among three folders: peer, server, and shared. <br />
Source code specific to the peer and server can be found in their respective folders, while code <br />
shared between both the peer and server can be found within the the shared folder. <br />

BUILD INSTRUCTIONS: Run "make" on the high-level Makefile. (note: this project uses recursive make) <br />

SERVER INSTRUCTIONS: The server can be started by issuing "./server.out <listening address> <listening port>" <br />

PEER INSTRUCTIONS:  The peer can be started by issuing "./peer.out <binding interface> <path-to-shared-folder> <server address> <server port>" <br />
The user-interface will then prompt the user for further actions.

EXAMPLE 1: P2P network with five peers. Tracking server is listening at 127.0.0.1 port 8888. Each peer has a dedicated folder<br />

./server.out 127.0.0.1 8888
./peer.out 127.0.0.1 "./testing/peer_1" 127.0.0.1 8888
./peer.out 127.0.0.1 "./testing/peer_2" 127.0.0.1 8888
./peer.out 127.0.0.1 "./testing/peer_3" 127.0.0.1 8888
./peer.out 127.0.0.1 "./testing/peer_4" 127.0.0.1 8888
./peer.out 127.0.0.1 "./testing/peer_5" 127.0.0.1 8888

TEST.SH: This is a script for launching a test environment.
