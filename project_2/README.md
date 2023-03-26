Overview: Source code for the bulletin-board is split among three folders: client, server, and shared. 
Source code specific to the client and server can be found in their respective folders, while code
shared between both the client and server can be found within the the shared folder.

Build Instructions: Run "make" on the high-level Makefile. (note: this project uses recursive make)

Client Instructions: The client can be started by issuing "./client.out <your name> <server address> <server port>".
The user-interface will then prompt the user for further actions.

Server Instructions: The server can be started by issuing "./server.out <listening address> <listening port>"

config.txt: The server-group consistency model and group IPs are specified in config.txt. Three examples are provided below.

EXAMPLE 1: Sequential consistency using 5 servers with 127.0.0.1:5555 as the primary

SEQUENTIAL
127.0.0.1:5555  <---- first server is always the primary <br />
127.0.0.1:6666
127.0.0.1:7777
127.0.0.1:8888
127.0.0.1:9999

EXAMPLE 2: Quorum consistency using 5 servers with 127.0.0.1:5555 as the primary, with Nr = 3 and Nw = 4

QUORUM;3;4
127.0.0.1:5555  <---- first server is always the primary
127.0.0.1:6666
127.0.0.1:7777
127.0.0.1:8888
127.0.0.1:9999

EXAMPLE 3: Read-your-write consistency using 5 servers with 127.0.0.1:5555 as the primary

READ-YOUR-WRITE
127.0.0.1:5555  <---- first server is always the primary
127.0.0.1:6666
127.0.0.1:7777
127.0.0.1:8888
127.0.0.1:9999