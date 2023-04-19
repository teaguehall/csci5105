#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>

#include "broadcaster.h"
#include "ui.h"
#include "request_handler.h"

#include "../shared/server_info.h"
#include "../shared/peer_info.h"
#include "../shared/connection_info.h"
#include "../shared/tcp.h"

ServerInfo server_info;
PeerInfo our_info;
char* shared_folder;

int main(int argc, char* argv[])
{
    // verify number of input arguments
    if(argc < 7 || argc > 7)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "ERROR: Unexpected number of input arguments received. Expected:\n");
        fprintf(stderr, " * REQUIRED: Peer - Binding Interface (e.g. 127.0.0.1)\n");
        fprintf(stderr, " * REQUIRED: Peer - Path to Shared Directory\n");
        fprintf(stderr, " * REQUIRED: Peer - Latitude Location (-90 to 90) \n");
        fprintf(stderr, " * REQUIRED: Peer - Latitude Location (-180 to 180) \n");
        fprintf(stderr, " * REQUIRED: Server - Address (xxx.xxx.xxx.xxx)\n");
        fprintf(stderr, " * REQUIRED: Server - Port\n");
        // TODO add optional args for test scenarios
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }

    // validate binding address for listener
    if(!tcp_IpAddrIsValid(argv[1]))
    {
        fprintf(stderr, "ERROR: Invalid binding interface for listening \"%s\" specified. Exitting...\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    else
    {
        memcpy(our_info.listening_addr, argv[1], strlen(argv[1]) + 1);
    }

    // verify shared folder exists
    shared_folder = argv[2];
    
    DIR *shared_dir;
    shared_dir = opendir(shared_folder);
    if(shared_dir == NULL)
    {
        fprintf(stderr, "Error opening shared folder \"%s\": %s. Exitting...\n", shared_folder, strerror(errno));
        exit(EXIT_FAILURE);
    }
    closedir(shared_dir);

    char* endptr;

    // validate latitude coordinate
    our_info.latitude = strtod(argv[3], &endptr);
    if(endptr == argv[3] || our_info.latitude < -90.0 || our_info.latitude > 90.0)
    {
        fprintf(stderr, "ERROR: Invalid latitude of node \"%s\" provided (-90.0 thru 90.0 allowed). Exitting...\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    // validate longitude coordinate
    our_info.longitude = strtod(argv[4], &endptr);
    if(endptr == argv[4] || our_info.longitude < -180.0 || our_info.longitude > 180.0)
    {
        fprintf(stderr, "ERROR: Invalid longitude of node \"%s\" provided (-180.0 thru 180.0 allowed). Exitting...\n", argv[4]);
        exit(EXIT_FAILURE);
    }

    // validate server address
    strcpy(server_info.listening_addr, argv[5]);
    if(!tcp_IpAddrIsValid(server_info.listening_addr))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided. Exitting...\n", argv[5]);
        exit(EXIT_FAILURE);
    }

    // validate server port
    server_info.listening_port = atoi(argv[6]);
    if(!tcp_PortIsValid(server_info.listening_port))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided. Exitting...\n", argv[6]);
        exit(EXIT_FAILURE);
    }

    int listener_socket = -1;

    // create listener socket (note: we will automatically find available port to listen on, hence the loop)
    for(our_info.listening_port = 1024; our_info.listening_port <= 65535; our_info.listening_port++)
    {
        if(!tcp_CreateListener(our_info.listening_addr, our_info.listening_port, &listener_socket))
        {
            break; // successfully created listener socket
        }

        listener_socket = -1; // reset socket to known value
    }

    // make sure we actually found a port
    if(listener_socket == -1)
    {
        fprintf(stderr, "ERROR: Peer failed to create listening socket. Exitting...\n");
        exit(EXIT_FAILURE);
    }

    // initialize broadcaster
    if(broadcaster_Init(&server_info, &our_info, shared_folder))
    {
        fprintf(stderr, "ERROR: Failed to initialize broadcaster module. Exitting...\n");
        exit(EXIT_FAILURE);
    }

    // initialize ui
    if(ui_Init(&server_info, &our_info, shared_folder))
    {
        fprintf(stderr, "ERROR: Failed to initialize broadcaster module. Exitting...\n");
        exit(EXIT_FAILURE);
    }

    ConnectionInfo connection;
    pthread_t thread_request_handler;

    // accept connections forever
    while(1)
    {
        // accept connection
        if(tcp_Accept(listener_socket, connection.remote_addr, &(connection.remote_port), &(connection.socket)))
        {
            printf("ERROR occurred while accepting connection\n");
            tcp_Disconnect(connection.socket);
            continue;
        }

        // spawn thread to handle request
        if(pthread_create(&thread_request_handler, NULL, requestHandler, (void*)(&connection)) != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn connection handler thread. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // shoud never reach here.. throw failure if it does
    return EXIT_FAILURE;
}
