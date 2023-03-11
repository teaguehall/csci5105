#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "client_handler.h"
#include "coordinate.h"

#include "../shared/tcp.h"
#include "../shared/msg.h"
#include "../shared/article.h"



int main(int argc, char * argv[])
{
    // verify number of input arguments
    if(argc < 5 || argc > 5)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "EROR: Unexpected number of input arguments received. Expected:\n");
        fprintf(stderr, " * REQUIRED: Server Listening Address (xxx.xxx.xxx.xxx)\n");
        fprintf(stderr, " * REQUIRED: Server Listening Port\n");
        fprintf(stderr, " * REQUIRED: Coordinator Server Address\n");
        fprintf(stderr, " * REQUIRED: Coordinator Server Port\n");
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

    // validate coordinator address
    if(!tcp_IpAddrIsValid(argv[3]))
    {
        fprintf(stderr, "ERROR: Invalid coordinator server address %s provided\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    // validate coordinator port
    if(!tcp_PortIsValid(atoi(argv[4])))
    {
        fprintf(stderr, "ERROR: Invalid coordinator server port %s provided\n", argv[4]);
        exit(EXIT_FAILURE);
    }
    
    // copy args to client info object
    ClientHandlerInfo client_handler_info;

    strcpy(client_handler_info.listening_address, argv[1]);
    client_handler_info.listening_port = atoi(argv[2]);

    // spawn client handler
    pthread_t thread_client_handler;
    if(pthread_create(&thread_client_handler, NULL, funcClientHandler, (void*)(&client_handler_info)) != 0)
    {
        fprintf(stderr, "ERROR: Failed to spawn client handler thread. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // copy args to coordinator object
    CoordinatorInfo coordination_info;

    strcpy(coordination_info.coord_address, argv[3]);
    coordination_info.coord_port = atoi(argv[4]);

    // if server listening address is the same as the coordinator server address, that means we're the coordinator!
    int is_coordinator = 0;
    if(strcmp(argv[1], argv[3]) == 0 && strcmp(argv[2], argv[4]) == 0)
    {
        is_coordinator = 1;
    }

    // spawn inter-server thread (depending on if this server is the coordinator or not)
    pthread_t thread_coordination;
    if(is_coordinator)
    {
        printf("Hello there!\n");
        if(pthread_create(&thread_coordination, NULL, funcCoordinator, (void*)(&coordination_info)) != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn coordinatOR thread. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if(pthread_create(&thread_coordination, NULL, funcCoordinated, (void*)(&coordination_info)) != 0)
        {
            fprintf(stderr, "ERROR: Failed to spawn coordinatED thread. %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // wait on threads (they should never finish)
    pthread_join(thread_coordination, NULL);
    pthread_join(thread_client_handler, NULL);
    
    // shoud never reach here.. throw failure if it does
    return EXIT_FAILURE;
}