#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "../shared/tcp.h"
#include "../shared/msg.h"
#include "../shared/article.h"

// coordinator thread info
typedef struct CoordinatorInfo
{
    char coord_address[128];
    int coord_port;

} CoordinatorInfo;

void* funcCoordinator(void *vargp);
void* funcCoordinated(void *vargp);

// client thread
typedef struct ClientHandlerInfo
{
    char listening_address[128];
    int listening_port;

} ClientHandlerInfo;

void* funcClientHandler(void *vargp);

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
    return 0;
    
    int listener_socket = -1;
    char local_addr[] = "127.0.0.1";
    int local_port = 5555;
    
    int remote_socket = -1;
    char remote_addr[64];
    int remote_port;

    char recv_msg[4096];
    char send_msg[4096];

    uint32_t msg_recv_type;
    uint32_t msg_recv_size;
    
    // create listener socket
    if(tcp_CreateListener(local_addr, local_port, &listener_socket))
    {
        printf("Failed to create listener socket\n");
        return -1;
    }
    
    // accept connections
    while(1)
    {        
        // accept connection
        if(tcp_Accept(listener_socket, remote_addr, &remote_port, &remote_socket))
        {
            printf("ERROR occurred while accepting connection\n");
            return -1;
        }

        // wait for header
        if(tcp_Recv(remote_socket, recv_msg, MSG_HEADER_OFFSET, 5))
        {
            printf("ERROR occurred while receiving message header\n");
            return -1;
        }

        // extract message header info
        if(msg_Parse_Header(recv_msg, &msg_recv_type, &msg_recv_size))
        {
            printf("Server received invalid header from client\n");
            return -1;
        }

        // read rest of message
        if(tcp_Recv(remote_socket, recv_msg + MSG_HEADER_OFFSET, msg_recv_size, 5))
        {
            printf("ERROR occurred while receiving message body\n");
            return -1;
        }

        // extract post message
        char author[4096];
        char title[4096];
        char contents[4096];

        if(msg_Parse_PostRequest(recv_msg, author, title, contents))
        {
            printf("ERROR while parsing Post request message");
            return -1;
        }

        // print debug message
        printf("Server received post: author = %s, title = %s, contents = %s\n", author, title, contents);

        // build response
        msg_Build_PostResponse(send_msg);

        // send response
        if(tcp_Send(remote_socket, send_msg, MSG_HEADER_OFFSET, 5))
        {
            printf("ERROR occurred while accepting connection\n");
            return -1;
        }

        // disconnect from client
        tcp_Disconnect(remote_socket);
    }

    return EXIT_SUCCESS;
}