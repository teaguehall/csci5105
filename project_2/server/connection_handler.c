#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "connection_handler.h"
#include "msg_handler.h"
#include "../shared/msg.h"
#include "../shared/tcp.h"
#include "../shared/database.h"
#include "file.h"

void* connectionHandler(void *vargp)
{
    ServerGroup server_group;
    int remote_socket;

    char rcvd_msg[MAX_MSG_SIZE];
    
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

    // send message to handler
    switch(msg_recv_type)
    {
        case MSG_TYPE_POST_REQUEST :
            msgHandler_PostRequest(&server_group, remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_READ_REQUEST :
            msgHandler_ReadRequest(&server_group, remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_CHOOSE_REQUEST :
            msgHandler_ChooseRequest(&server_group, remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_REPLY_REQUEST :
            msgHandler_ReplyRequest(&server_group, remote_socket, rcvd_msg);
            break;
        case MSG_TYPE_DB_PUSH_REQUEST :
            msgHandler_DbPushRequest(&server_group, remote_socket, rcvd_msg);
            break;
        default:
            fprintf(stderr, "ERROR: Server received unrecognized message %d\n", msg_recv_type);
            return NULL;
    }

    // disconnect from client after message was processed
    tcp_Disconnect(remote_socket);
    return NULL;

}