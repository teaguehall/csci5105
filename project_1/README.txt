Background: The majority of user code is featured in client.c, server_routines.c, and article.c. The rest of the source code files were automatically generated from communicate.x using RPCGEN.

Build Instructions: Run 'make' on the provided Makefile. 

Quick Start: Start up an instance of server.out on a lab machine. Start up an instance of client.out on the same or different lab machine with the server machine as an input argument. The client application will prompt you for user input commands.

Tests: Several tests can be found in the test.sh bash script. These tests are described on script start-up along with expected outputs. Test cases are not exhuastive, however, they target the primary operating modes of the pub/sub system. 