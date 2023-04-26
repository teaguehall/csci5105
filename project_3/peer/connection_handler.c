#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "connection_handler.h"
#include "load_tracker.h"
#include "file.h"

#include "../shared/connection_info.h"
#include "../shared/assumptions.h"
#include "../shared/server_info.h"
#include "../shared/tcp.h"
#include "../shared/msg.h"

extern ServerInfo g_server_info;
extern PeerInfo g_our_info;
extern char g_shared_folder[512];

void msgHandler_GetLoadRequest(ConnectionInfo* connection_info, char* msg)
{
    time_t t;
    char response[MAX_MSG_SIZE_BYTES];

    // simulate latency between 100 and 1000ms
    srand((unsigned) time(&t));
    int latency_ms = (rand() % 901) + 100;
    usleep(latency_ms * 1000);
    
    // grab current loads
    int throughput = loadTracker_GetThroughPut();

    // build response
    msg_Build_GetLoadResponse(response, throughput);

    // send response
    if(tcp_Send(connection_info->socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Peer failed to send GET-LOAD-RESPONSE\n");
    }
}

void msgHandler_DownloadRequest(ConnectionInfo* connection_info, char* msg)
{
    time_t t;
    char error_msg[1024];
    char response[MAX_MSG_SIZE_BYTES];
    char requested_file_name[512];
    FileInfo file_info;
    char file_data[MAX_FILE_SIZE_BYTES];

    // parse received message
    if(msg_Parse_DownloadRequest(msg, requested_file_name))
    {
        msg_Build_ErrorResponse(response, "Peer received ill-formatted DOWNLOAD-REQUEST message");
    }
    else
    {
        // attempt to read file from disk
        if(file_reader(g_shared_folder, requested_file_name, &file_info, file_data))
        {
            sprintf(error_msg, "Peer could not retrieve request file \"%s\"", requested_file_name);
            msg_Build_ErrorResponse(response, "Server has reached max peer capacity. Wait until another peer leaves the network.");
        }
        else
        {
            // simulate data corruption by modifying byte 50% of the time 
            if(rand() % 2)
            {
                file_data[0]++;
            }

            // build download response
            msg_Build_DownloadResponse(response, &file_info, file_data);
        }
    }

    // increment load counter
    loadTracker_Add();

    // simulate latency between 100 and 1000ms
    srand((unsigned) time(&t));
    int latency_ms = (rand() % 901) + 100;
    usleep(latency_ms * 1000);

    // send response
    if(tcp_Send(connection_info->socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Server failed to send DOWNLOAD-REQUEST-RESPONSE\n");
    }

    // decrement load counter
    loadTracker_Sub();
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
        case MSG_TYPE_GET_LOAD_REQUEST :
            msgHandler_GetLoadRequest(&connection, rcvd_msg);
            break;
        case MSG_TYPE_DOWNLOAD_REQUEST :
            msgHandler_DownloadRequest(&connection, rcvd_msg);
            break;
        default:
            msgHandler_UnknownRequest(&connection, msg_recv_type);
            break;
    }

    // disconnect from client after message was processed
    tcp_Disconnect(connection.socket);
    return NULL;
}
