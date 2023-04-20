#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>

#include "broadcaster.h"
#include "ui.h"
#include "connection_handler.h"

#include "../shared/server_info.h"
#include "../shared/peer_info.h"
#include "../shared/connection_info.h"
#include "../shared/tcp.h"

ServerInfo g_server_info;
PeerInfo g_our_info;
char g_shared_folder[512];

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
        memcpy(g_our_info.listening_addr, argv[1], strlen(argv[1]) + 1);
    }

    // verify shared folder exists
    strncpy(g_shared_folder, argv[2], sizeof(g_shared_folder));
    g_shared_folder[sizeof(g_shared_folder) - 1] = 0; // null term
    
    DIR *shared_dir;
    shared_dir = opendir(g_shared_folder);
    if(shared_dir == NULL)
    {
        fprintf(stderr, "Error opening shared folder \"%s\": %s. Exitting...\n", g_shared_folder, strerror(errno));
        exit(EXIT_FAILURE);
    }
    closedir(shared_dir);

    char* endptr;

    // validate latitude coordinate
    g_our_info.latitude = strtod(argv[3], &endptr);
    if(endptr == argv[3] || g_our_info.latitude < -90.0 || g_our_info.latitude > 90.0)
    {
        fprintf(stderr, "ERROR: Invalid latitude of node \"%s\" provided (-90.0 thru 90.0 allowed). Exitting...\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    // validate longitude coordinate
    g_our_info.longitude = strtod(argv[4], &endptr);
    if(endptr == argv[4] || g_our_info.longitude < -180.0 || g_our_info.longitude > 180.0)
    {
        fprintf(stderr, "ERROR: Invalid longitude of node \"%s\" provided (-180.0 thru 180.0 allowed). Exitting...\n", argv[4]);
        exit(EXIT_FAILURE);
    }

    // validate server address
    strcpy(g_server_info.listening_addr, argv[5]);
    if(!tcp_IpAddrIsValid(g_server_info.listening_addr))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided. Exitting...\n", argv[5]);
        exit(EXIT_FAILURE);
    }

    // validate server port
    g_server_info.listening_port = atoi(argv[6]);
    if(!tcp_PortIsValid(g_server_info.listening_port))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided. Exitting...\n", argv[6]);
        exit(EXIT_FAILURE);
    }

    int listener_socket = -1;

    // create listener socket (note: we will automatically find available port to listen on, hence the loop)
    for(g_our_info.listening_port = 1024; g_our_info.listening_port <= 65535; g_our_info.listening_port++)
    {
        if(!tcp_CreateListener(g_our_info.listening_addr, g_our_info.listening_port, &listener_socket))
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
    if(broadcaster_Init())
    {
        fprintf(stderr, "ERROR: Failed to initialize broadcaster module. Exitting...\n");
        exit(EXIT_FAILURE);
    }

    // initialize ui
    if(ui_Init())
    {
        fprintf(stderr, "ERROR: Failed to initialize broadcaster module. Exitting...\n");
        exit(EXIT_FAILURE);
    }

    pthread_t thread_connection_handler;

    // accept connections forever
    while(1)
    { 
        // malloc new connection object (connection handler will be responsible for freeing this)
        ConnectionInfo* connection = malloc(sizeof(ConnectionInfo));
        if(connection == NULL)
        {
            fprintf(stderr, "FATAL: Failed to malloc new connection object for connection handler thread\n");
            exit(EXIT_FAILURE);
        }

        // accept connection
        if(tcp_Accept(listener_socket, connection->remote_addr, &(connection->remote_port), &(connection->socket)))
        {
            
            printf("ERROR occurred while accepting connection\n");
            tcp_Disconnect(connection->socket);
            continue;
        }

        // spawn thread to handle request
        if(pthread_create(&thread_connection_handler, NULL, connectionHandler, (void*)(connection)) != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn connection handler thread. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // shoud never reach here.. throw failure if it does
    return EXIT_FAILURE;
}
