#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "handler_client.h"
#include "coordinate.h"
#include "file.h"

#include "../shared/tcp.h"
#include "../shared/msg.h"
#include "../shared/article.h"

ServerGroup server_group;
int is_coordinator;

int main(int argc, char * argv[])
{
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

        // off load to connection handler
    }




    // accept connections
    while(1)
    {        
        // accept connection
        if(tcp_Accept(listener_socket, remote_addr, &remote_port, &remote_socket))
        {
            printf("ERROR occurred while accepting connection\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // wait for header
        if(tcp_Recv(remote_socket, recv_msg, MSG_HEADER_OFFSET, 5))
        {
            printf("ERROR occurred while receiving message header\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // extract message header info
        if(msg_Parse_Header(recv_msg, &msg_recv_type, &msg_recv_id, &msg_recv_size))
        {
            printf("Server received invalid header from client\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // read rest of message
        if(tcp_Recv(remote_socket, recv_msg + MSG_HEADER_OFFSET, msg_recv_size, 5))
        {
            printf("ERROR occurred while receiving message body\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // handle rest of message depending on type
        switch(msg_recv_type)
        {
            case MSG_TYPE_POST_REQUEST :
                handlePostRequest(remote_socket, recv_msg);
                break;
            case MSG_TYPE_READ_REQUEST :
                handleReadRequest(remote_socket, recv_msg);
                break;
            case MSG_TYPE_CHOOSE_REQUEST :
                handleChooseRequest(remote_socket, recv_msg);
                break;
            case MSG_TYPE_REPLY_REQUEST :
                handleReplyRequest(remote_socket, recv_msg);
                break;
            default:
                fprintf(stderr, "ERROR: Server received unrecognized message\n");
                continue;
        }

        // disconnect from client
        tcp_Disconnect(remote_socket);

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

    // TODO

    //// if server listening address is the same as the coordinator server address, that means we're the coordinator!
    //int is_coordinator = 0;
    //if(strcmp(argv[1], argv[3]) == 0 && strcmp(argv[2], argv[4]) == 0)
    //{
    //    is_coordinator = 1;
    //}
    int is_coordinator = 0;

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
