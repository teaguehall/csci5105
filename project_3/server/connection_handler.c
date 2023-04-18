#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "connection_handler.h"

#include "../shared/assumptions.h"
#include "../shared/tcp.h"
#include "../shared/msg.h"

void msgHandler_DiscoverRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_DiscoverRequest\n");
}

void msgHandler_FindRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_FindRequest\n");
}

void msgHandler_UpdateListRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_UpdateListRequest\n");
}

void msgHandler_PingRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_PingRequest\n");
}

void msgHandler_UnknownRequest(ConnectionInfo* connection_info, char* msg)
{
    printf("TODO - msgHandler_UnknownRequest\n");
}

void* connectionHandler(void* vargp)
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
        case MSG_TYPE_DISCOVER_REQUEST :
            msgHandler_DiscoverRequest(&connection, rcvd_msg);
            break;
        case MSG_TYPE_FIND_REQUEST :
            msgHandler_FindRequest(&connection, rcvd_msg);
            break;
        case MSG_TYPE_UPDATE_LIST_REQUEST :
            msgHandler_UpdateListRequest(&connection, rcvd_msg);
            break;
        case MSG_TYPE_PING_REQUEST :
            msgHandler_PingRequest(&connection, rcvd_msg);
            break;
        default:
            msgHandler_UnknownRequest(&connection, rcvd_msg);
            break;
    }

    // disconnect from client after message was processed
    tcp_Disconnect(connection.socket);
    return NULL;
}