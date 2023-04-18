#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "request_handler.h"

#include "../shared/connection_info.h"
#include "../shared/assumptions.h"
#include "../shared/tcp.h"
#include "../shared/msg.h"

void msgHandler_GetLoadRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_GetLoadRequest\n");
}

void msgHandler_DownloadRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_DownloadRequest\n");
}

void msgHandler_UnknownRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_UnknownRequest\n");
}

void* requestHandler(void* vargp)
{
    ConnectionInfo connection;
    char rcvd_msg[MAX_MSG_SIZE_BYTES];

    // cast arguments to local variables
    memcpy(&connection, vargp, sizeof(connection));
    
    uint32_t msg_recv_type;
    int32_t msg_recv_id;
    uint32_t msg_recv_size;

    // read message header
    if(tcp_Recv(connection.socket, rcvd_msg, MSG_HEADER_SIZE, 5))
    {
        printf("ERROR occurred while receiving message header\n");
        tcp_Disconnect(connection.socket);
        return NULL;
    }

    // extract message header info
    if(msg_Parse_Header(rcvd_msg, &msg_recv_type, &msg_recv_id, &msg_recv_size))
    {
        printf("Server received invalid header from client\n");
        tcp_Disconnect(connection.socket);
        return NULL;
    }

    // read rest of message
    if(tcp_Recv(connection.socket, rcvd_msg + MSG_HEADER_SIZE, msg_recv_size, 5))
    {
        printf("ERROR occurred while receiving message body\n");
        tcp_Disconnect(connection.socket);
        return NULL;
    }

    // process message
    switch(msg_recv_type)
    {
        case MSG_TYPE_GET_LOAD_REQUEST :
            msgHandler_GetLoadRequest(&connection, rcvd_msg);
            break;
        case MSG_TYPE_DOWNLOAD_REQUEST :
            msgHandler_DownloadRequest(&connection, rcvd_msg);
            break;
        default:
            msgHandler_UnknownRequest(&connection, rcvd_msg);
            break;
    }

    // disconnect from client after message was processed
    tcp_Disconnect(connection.socket);
    return NULL;
}
