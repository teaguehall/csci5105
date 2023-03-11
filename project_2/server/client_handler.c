#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "client_handler.h"
#include "../shared/msg.h"

static void handlePostRequest(int socket, char* msg_rcvd)
{

}

static void handleReadRequest(int socket, char* msg_rcvd)
{

}

static void handleChooseRequest(int socket, char* msg_rcvd)
{

}

static void handleReplyRequest(int socket, char* msg_rcvd)
{

}

void* funcClientHandler(void *vargp)
{
    uint32_t msg_recv_type;
    uint32_t msg_recv_size;
    
    int listener_socket = -1;
    int remote_socket = -1;
    char remote_addr[128];
    int remote_port;

    char recv_msg[4096];
    char send_msg[4096];
    
    // cast input args
    char* listening_addr = ((ClientHandlerInfo*)(vargp))->listening_address;
    int listening_port = ((ClientHandlerInfo*)(vargp))->listening_port;  
    
    // create listener socket
    if(tcp_CreateListener(listening_addr, listening_port, &listener_socket))
    {
        printf("Failed to create listener socket\n");
        exit(EXIT_FAILURE);
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
        if(msg_Parse_Header(recv_msg, &msg_recv_type, &msg_recv_size))
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
                handlePostRequest(remote_socket);
            case MSG_TYPE_READ_REQUEST :
                handlePostRequest(remote_socket);
            case MSG_TYPE_CHOOSE_REQUEST :
                handlePostRequest(remote_socket);
            case MSG_TYPE_REPLY_REQUEST :
                handlePostRequest(remote_socket);
            default:
                fprintf(stderr, "ERROR: Server received unrecognized message\n");
                continue;
        }

        // handle rest of message


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



    // success
    return NULL;
}

