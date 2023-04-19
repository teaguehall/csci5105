#include <stdio.h>

#include "send.h"

#include "../shared/assumptions.h"
#include "../shared/tcp.h"
#include "../shared/msg.h"

// performs message send/recv operation. return 0 on success, -1 on error
static int send_recv(const char* address, int port, const char* msg_send, char* msg_recv)
{
    int _socket = -1;
    
    uint32_t msg_send_size;
    int32_t msg_send_id;
    uint32_t msg_send_type;
    uint32_t msg_recv_size;
    int32_t msg_recv_id;
    uint32_t msg_recv_type;
    
    // connect to remote host
    if(tcp_Connect(address, port, &_socket))
    {
        fprintf(stderr, "ERROR: Failed to connect to remote host %s:%d\n", address, port);
        return -1;
    }

    // calculate message size (contents + header)
    if(msg_Parse_Header(msg_send, &msg_send_type, &msg_send_id, &msg_send_size))
    {
        fprintf(stderr, "ERROR: Attempted to send invalid message header to remote host\n");
    }
    msg_send_size += MSG_HEADER_SIZE;

    // send message
    if(tcp_Send(_socket, msg_send, msg_send_size, 5))
    {
        fprintf(stderr, "ERROR: Failed to send message\n");
        return -1; 
    }

    // read response header
    if(tcp_Recv(_socket, msg_recv, MSG_HEADER_SIZE, 5))
    {
        fprintf(stderr, "ERROR: Failed to receive message header response from remote host.\n");
        return -1; 
    }

    // parse recv header
    if(msg_Parse_Header(msg_recv, &msg_recv_type, &msg_recv_id, &msg_recv_size))
    {
        fprintf(stderr, "ERROR: Invalid message header received from remote host\n");
        return -1; 
    }

    // read response message
    if(tcp_Recv(_socket, msg_recv + MSG_HEADER_SIZE, msg_recv_size, 5))
    {
        fprintf(stderr, "ERROR: Failed to receive message body from remote host.\n");
        return -1; 
    }

    // check if received message type was an error message
    char error_string[4096];
    if(msg_recv_type == MSG_TYPE_ERROR_RESPONSE)
    {
        msg_Parse_ErrorResponse(msg_recv, error_string);
        
        fprintf(stderr, "Server Responded with ERROR: %s\n", error_string);
        return -1;
    }

    // check for unexpected recieve number
    if(msg_recv_type != (msg_send_type + 1))
    {
        fprintf(stderr, "ERROR: Unexpected message type received. Expected %u, Received %u\n", msg_send_type + 1, msg_recv_type);
        return -1;
    }

    // disconnect from server
    if(tcp_Disconnect(_socket))
    {
        fprintf(stderr, "ERROR: Failed to disconnect from remote host %s:%d\n", address, port);
        return -1;
    }

    // success
    return 0;
}

int send_PingRequest(const ServerInfo* server, const PeerInfo* us, int* out_recognized)
{
    char send_msg[MAX_MSG_SIZE_BYTES];
    char recv_msg[MAX_MSG_SIZE_BYTES];

    // build message
    if(msg_Build_PingRequest(send_msg, us))
    {
        return -1; 
    }

    // transmit message
    if(send_recv(server->listening_addr, server->listening_port, send_msg, recv_msg))
    {
        return -1;
    }
 
    // parse response
    if(msg_Parse_PingResponse(recv_msg, out_recognized))
    {
        return -1;
    }

    // success
    return 0;
}

int send_UpdateListRequest(const ServerInfo* server, const PeerInfo* us, size_t file_count, const FileInfo files[])
{
    char send_msg[MAX_MSG_SIZE_BYTES];
    char recv_msg[MAX_MSG_SIZE_BYTES];
    
    // build message
    if(msg_Build_UpdateListRequest(send_msg, us, file_count, files))
    {
        return -1;
    }

    // transmit message
    if(send_recv(server->listening_addr, server->listening_port, send_msg, recv_msg))
    {
        return -1;
    }

    // success
    return 0;
}

int send_DiscoverRequest(const ServerInfo* server, int* out_file_count, FileInfo out_files[])
{
    char send_msg[MAX_MSG_SIZE_BYTES];
    char recv_msg[MAX_MSG_SIZE_BYTES];

    // build message
    if(msg_Build_DiscoverRequest(send_msg))
    {
        return -1;
    }

    // transmit message
    if(send_recv(server->listening_addr, server->listening_port, send_msg, recv_msg))
    {
        return -1;
    }

    // parse response
    if(msg_Parse_DiscoverResponse(recv_msg, out_file_count, out_files))
    {
        return -1;
    }

    // success
    return 0;
}

int send_FindRequest(const ServerInfo* server, const char* file_name, int* out_peer_count, PeerInfo out_peers[])
{
    char send_msg[MAX_MSG_SIZE_BYTES];
    char recv_msg[MAX_MSG_SIZE_BYTES];

    // build message
    if(msg_Build_FindRequest(send_msg, file_name))
    {
        return -1;
    }

    // transmit message
    if(send_recv(server->listening_addr, server->listening_port, send_msg, recv_msg))
    {
        return -1;
    }

    // parse response
    if(msg_Parse_FindResponse(recv_msg, out_peer_count, out_peers))
    {
        return -1;
    }

    // success
    return 0;
}

int send_GetLoadRequest(const PeerInfo* peer, int* out_loads)
{
    char send_msg[MAX_MSG_SIZE_BYTES];
    char recv_msg[MAX_MSG_SIZE_BYTES];

    // build message
    if(msg_Build_GetLoadRequest(send_msg))
    {
        return -1;
    }

    // transmit message
    if(send_recv(peer->listening_addr, peer->listening_port, send_msg, recv_msg))
    {
        return -1;
    }

    // parse response
    if(msg_Parse_GetLoadResponse(recv_msg, out_loads))
    {
        return -1;
    }

    // success
    return 0;
}

int send_DownloadRequest(const PeerInfo* peer, const char* file_name, FileInfo* out_file_info, char* out_file_data)
{
    char send_msg[MAX_MSG_SIZE_BYTES];
    char recv_msg[MAX_MSG_SIZE_BYTES];

    // build message
    if(msg_Build_DownloadRequest(send_msg, file_name))
    {
        return -1;
    }

    // transmit message
    if(send_recv(peer->listening_addr, peer->listening_port, send_msg, recv_msg))
    {
        return -1;
    }

    // parse response
    if(msg_Parse_DownloadResponse(recv_msg, out_file_info, out_file_data))
    {
        return -1;
    }

    // success
    return 0;    
}
