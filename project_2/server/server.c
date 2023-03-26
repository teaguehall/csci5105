#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "handler_connection.h"
#include "file.h"

#include "../shared/tcp.h"
#include "../shared/msg.h"
#include "../shared/article.h"

ServerGroup server_group;
int is_coordinator;

int main(int argc, char * argv[])
{
    pthread_t thread_connection;

    int listener_socket = -1;
    int remote_socket = -1;
    char remote_addr[128];
    int remote_port;
    
    // verify number of input arguments
    if(argc < 3 || argc > 3)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "EROR: Unexpected number of input arguments received. Expected:\n");
        fprintf(stderr, " * REQUIRED: Server Listening Address (xxx.xxx.xxx.xxx)\n");
        fprintf(stderr, " * REQUIRED: Server Listening Port\n");
        fprintf(stderr, "\n");

        exit(EXIT_FAILURE);
    }

    // validate listener address
    if(!tcp_IpAddrIsValid(argv[1]))
    {
        fprintf(stderr, "ERROR: Invalid server listening address %s provided\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // validate listener port
    if(!tcp_PortIsValid(atoi(argv[2])))
    {
        fprintf(stderr, "ERROR: Invalid server listening port %s provided\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    // parse server group file
    if(file_ParseServerGroup("./config.txt", &server_group))
    {
        fprintf(stderr, "ERROR: Error occurred while parsing server group config file.");
        exit(EXIT_FAILURE);
    }

    // determine if we are the coordinator
    if(strcmp(argv[1], server_group.primary.address) == 0 && atoi(argv[2]) == server_group.primary.port)
    {
        printf("we ARE the coordinator\n");
        is_coordinator = 1;
    }
    else
    {
        printf("we ARE NOT the coordinator\n");
        is_coordinator = 0;
    }

    // print start-up information
    

    // copy info to connection thread input args
    ConnectionHandlerInfo connection_info;
    memcpy(&(connection_info.server_group), &server_group, sizeof(server_group));

    // create listener socket
    if(tcp_CreateListener(argv[1], atoi(argv[2]), &listener_socket))
    {
        printf("Failed to create listener socket\n");
        exit(EXIT_FAILURE);
    }

    // accept connections forever
    while(1)
    {
        // accept connection
        if(tcp_Accept(listener_socket, remote_addr, &remote_port, &remote_socket))
        {
            printf("ERROR occurred while accepting connection\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // create connection request info
        connection_info.remote_socket = remote_socket;
        
        // create new thread for connection
        if(pthread_create(&thread_connection, NULL, connectionHandler, (void*)(&connection_info)) != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn connection handler thread. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // shoud never reach here.. throw failure if it does
    return EXIT_FAILURE;
}
