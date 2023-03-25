#include "connection_handler.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "handler_client.h"
#include "../shared/msg.h"
#include "../shared/tcp.h"
#include "database.h"
#include "server_msg.h"

void* connectionHandler(void *vargp)
{
    int remote_socket;
    char remote_addr[256];
    int remote_port;

    char rcvd_msg[MAX_SERVER_MSG_SIZE];
    
    uint32_t msg_recv_type;
    int32_t msg_recv_id;
    uint32_t msg_recv_size;


    // cast/copy arguments to local variables
    remote_socket = ((ConnectionHandlerInfo*)(vargp))->remote_socket;
    strcpy(remote_addr, ((ConnectionHandlerInfo*)(vargp))->remote_addr);
    remote_port = ((ConnectionHandlerInfo*)(vargp))->remote_port;

    // read message header
    if(tcp_Recv(remote_socket, rcvd_msg, MSG_HEADER_OFFSET, 5))
    {
        printf("ERROR occurred while receiving message header\n");
        tcp_Disconnect(remote_socket);
        return NULL;
    }

    // extract message header info
    if(msg_Parse_Header(rcvd_msg, &msg_recv_type, &msg_recv_id, &msg_recv_size))
    {
        printf("Server received invalid header from client\n");
        tcp_Disconnect(remote_socket);
        return NULL;
    }

    // read rest of message
    if(tcp_Recv(remote_socket, rcvd_msg + MSG_HEADER_OFFSET, msg_recv_size, 5))
    {
        printf("ERROR occurred while receiving message body\n");
        tcp_Disconnect(remote_socket);
        return NULL;
    }

    // handle message
    switch(msg_recv_type)
    {
        case MSG_TYPE_POST_REQUEST :
            handlePostRequest(remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_READ_REQUEST :
            handleReadRequest(remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_CHOOSE_REQUEST :
            handleChooseRequest(remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_REPLY_REQUEST :
            handleReplyRequest(remote_socket, rcvd_msg);
            break;
        default:
            fprintf(stderr, "ERROR: Server received unrecognized message\n");
            return NULL;
    }

    // success
    tcp_Disconnect(remote_socket);
    return NULL;

}