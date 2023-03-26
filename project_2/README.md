OVERVIEW: Source code for the bulletin-board is split among three folders: client, server, and shared. <br />
Source code specific to the client and server can be found in their respective folders, while code <br />
shared between both the client and server can be found within the the shared folder. <br />

BUILD INSTRUCTIONS: Run "make" on the high-level Makefile. (note: this project uses recursive make) <br />

CLIENT INSTRUCTIONS: The client can be started by issuing "./client.out <your name> <server address> <server port>". <br />
The user-interface will then prompt the user for further actions.

SERVER INSTRUCTIONS: The server can be started by issuing "./server.out <listening address> <listening port>" <br />

CONFIG.TXT: The server-group consistency model and group IPs are specified in config.txt. Three examples are provided below. <br />

EXAMPLE 1: Sequential consistency using 5 servers with 127.0.0.1:5555 as the primary <br />

SEQUENTIAL <br />
127.0.0.1:5555  <---- first server is always the primary <br />
127.0.0.1:6666 <br />
127.0.0.1:7777 <br />
127.0.0.1:8888 <br />
127.0.0.1:9999 <br />

EXAMPLE 2: Quorum consistency using 5 servers with 127.0.0.1:5555 as the primary, with Nr = 3 and Nw = 4 <br />

QUORUM;3;4 <br />
127.0.0.1:5555  <---- first server is always the primary <br />
127.0.0.1:6666 <br />
127.0.0.1:7777 <br />
127.0.0.1:8888 <br />
127.0.0.1:9999 <br />

EXAMPLE 3: Read-your-write consistency using 5 servers with 127.0.0.1:5555 as the primary <br />

READ-YOUR-WRITE <br />
127.0.0.1:5555  <---- first server is always the primary <br />
127.0.0.1:6666 <br />
127.0.0.1:7777 <br />
127.0.0.1:8888 <br />
127.0.0.1:9999 <br />

SERVER_CONTROL.SH: This is a helper script for launching multiple servers in the background. Note that this file needs to the match the servers specified in config.txt