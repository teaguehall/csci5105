#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "connection_handler.h"
#include "database.h"

#include "../shared/assumptions.h"
#include "../shared/tcp.h"
#include "../shared/msg.h"
#include "../shared/file_info.h"

void msgHandler_DiscoverRequest(ConnectionInfo* connection_info, char* msg)
{
    char response[MAX_MSG_SIZE_BYTES];
    FileInfo files[MAX_PEERS_IN_NETWORK * MAX_FILES_PER_PEER];
    int num_of_files;
    
    // query files against database and build response message
    if(db_DiscoverFiles(&num_of_files, files))
    {
        msg_Build_ErrorResponse(response, "Internal server error in db_DiscoverFiles()");
    }
    else
    {
        msg_Build_DiscoverResponse(response, num_of_files, files);
    }

    // send response
    if(tcp_Send(connection_info->socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Server failed to send DISCOVER-RESPONSE\n");
    }  
}

void msgHandler_FindRequest(ConnectionInfo* connection_info, char* msg)
{
    char response[MAX_MSG_SIZE_BYTES];
    char file_name[MAX_FILE_NAME_SIZE];
    PeerInfo peers[MAX_PEERS_IN_NETWORK];
    int num_of_peers;

    // parse received message
    if(msg_Parse_FindRequest(msg, file_name))
    {
        msg_Build_ErrorResponse(response, "Server received ill-formatted FIND-REQUEST message");
    }
    else
    {
        // query files against database and build response message
        if(db_FindFile(file_name, &num_of_peers, peers))
        {
            msg_Build_ErrorResponse(response, "Internal server error in db_FindFile()");
        }
        else
        {
            msg_Build_FindResponse(response, num_of_peers, peers);
        }
    }

    // send response
    if(tcp_Send(connection_info->socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Server failed to send FIND-RESPONSE\n");
    }
}

void msgHandler_UpdateListRequest(ConnectionInfo* connection_info, char* msg)
{
    char response[MAX_MSG_SIZE_BYTES];
    PeerInfo peer;
    int num_of_files;
    FileInfo files[MAX_FILES_PER_PEER];

    // parse received message
    if(msg_Parse_UpdateListRequest(msg, &peer, &num_of_files, files))
    {
        msg_Build_ErrorResponse(response, "Server received ill-formatted UPDATELIST-REQUEST message");
    }
    else
    {
        // add peer info to database
        if(db_AddPeer(&peer, num_of_files, files))
        {
            msg_Build_ErrorResponse(response, "Server has reached max peer capacity. Wait until another peer leaves the network.");
        }
        else
        {
            msg_Build_UpdateListResponse(response);
        }
    }

    // send response
    if(tcp_Send(connection_info->socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Server failed to send UPDATE-LIST-RESPONSE\n");
    }
}

void msgHandler_PingRequest(ConnectionInfo* connection_info, char* msg)
{
    char response[MAX_MSG_SIZE_BYTES];
    PeerInfo peer;
    int recognized;

    // parse received message
    if(msg_Parse_PingRequest(msg, &peer))
    {
        msg_Build_ErrorResponse(response, "Server received ill-formatted PING-REQUEST message");
    }
    else
    {
        // update keep alive
        if(db_KeepAlive(&peer))
        {
            recognized = 0;
        }
        else
        {
            recognized = 1;
        }

        // build response
        msg_Build_PingResponse(response, recognized);
    }

    // send response
    if(tcp_Send(connection_info->socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Server failed to send PING-RESPONSE\n");
    }
}

void msgHandler_UnknownRequest(ConnectionInfo* connection_info, uint32_t msg_type)
{
    char response[MAX_MSG_SIZE_BYTES];
    char error_msg[512];

    // build error response
    sprintf(error_msg, "Unknown message type \"%u\" received from client", msg_type);
    msg_Build_ErrorResponse(response, error_msg);

    // send response
    if(tcp_Send(connection_info->socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Server failed to send UNKNOWN-REQUEST-RESPONSE\n");
    }
}

void* connectionHandler(void* vargp)
{    
    ConnectionInfo connection;
    char rcvd_msg[MAX_MSG_SIZE_BYTES];

    // cast arguments to local variables
    memcpy(&connection, vargp, sizeof(connection));

    // free the temporary connection object that was passed in
    free(vargp);
    
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
            msgHandler_UnknownRequest(&connection, msg_recv_type);
            break;
    }

    // disconnect from client after message was processed
    tcp_Disconnect(connection.socket);
    return NULL;
}