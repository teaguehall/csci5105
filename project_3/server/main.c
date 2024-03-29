#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include "connection_handler.h"
#include "database.h"

#include "../shared/tcp.h"

int main(int argc, char * argv[])
{
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

    int listener_socket = -1;

    // initialize database
    if(db_Init())
    {
        fprintf(stderr, "ERROR: Database failed to initialize\n");
        exit(EXIT_FAILURE);
    }

    // create listener socket
    if(tcp_CreateListener(argv[1], atoi(argv[2]), &listener_socket))
    {
        printf("Failed to create listener socket\n");
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
        if(tcp_Accept(listener_socket, connection->remote_address, &(connection->remote_port), &(connection->socket)))
        {
            printf("ERROR occurred while accepting connection\n");
            tcp_Disconnect(connection->socket);
            free(connection);
            continue;
        }

        // create new thread for connection
        if(pthread_create(&thread_connection_handler, NULL, connectionHandler, (void*)(connection)) != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn connection handler thread. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // shoud never reach here.. throw failure if it does
    return EXIT_FAILURE;
}
