#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "handler_connection.h"
#include "handler_request.h"
#include "../shared/msg.h"
#include "../shared/tcp.h"
#include "database.h"
#include "server_msg.h"
#include "file.h"

void* connectionHandler(void *vargp)
{
    ServerGroup server_group;
    int remote_socket;

    char rcvd_msg[MAX_SERVER_MSG_SIZE];
    
    uint32_t msg_recv_type;
    int32_t msg_recv_id;
    uint32_t msg_recv_size;

    // cast/copy arguments to local variables
    server_group = ((ConnectionHandlerInfo*)(vargp))->server_group;
    remote_socket = ((ConnectionHandlerInfo*)(vargp))->remote_socket;

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
            handlePostRequest(&server_group, remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_READ_REQUEST :
            handleReadRequest(&server_group, remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_CHOOSE_REQUEST :
            handleChooseRequest(&server_group, remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_REPLY_REQUEST :
            handleReplyRequest(&server_group, remote_socket, rcvd_msg);
            break;
        default:
            fprintf(stderr, "ERROR: Server received unrecognized message\n");
            return NULL;
    }

    // success
    tcp_Disconnect(remote_socket);
    return NULL;

}