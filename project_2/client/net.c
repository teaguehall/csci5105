#include "net.h"
#include <stdio.h>
#include "../shared/msg.h"
#include "../shared/tcp.h"

// sends message to server and receives response. return 0 on success, -1 on error
int net_SendRecv(ServerInfo server, char* msg_send, char* msg_recv)
{
    int _socket = -1;
    
    uint32_t msg_send_size;
    uint32_t msg_send_type;
    uint32_t msg_recv_size;
    uint32_t msg_recv_type;
    
    // connect to server
    if(tcp_Connect(server.address, server.port, &_socket))
    {
        fprintf(stderr, "ERROR: Failed to connect to server %s:%d\n", server.address, server.port);
        return -1;
    }

    // calculate message size (contents + header)
    if(msg_Parse_Header(msg_send, &msg_send_type, &msg_send_size))
    {
        fprintf(stderr, "ERROR: Attempted to send invalid message header to server\n");
    }
    msg_send_size += MSG_HEADER_OFFSET;

    // send message
    if(tcp_Send(_socket, msg_send, msg_send_size, 5))
    {
        fprintf(stderr, "ERROR: Failed to send message\n");
        return -1; 
    }

    // read response header
    if(tcp_Recv(_socket, msg_recv, MSG_HEADER_OFFSET, 5))
    {
        fprintf(stderr, "ERROR: Failed to receive message header response from server.\n");
        return -1; 
    }

    // parse recv header
    if(msg_Parse_Header(msg_recv, &msg_recv_type, &msg_recv_size))
    {
        fprintf(stderr, "ERROR: Invalid message header received from server\n");
        return -1; 
    }

    // read response message
    if(tcp_Recv(_socket, msg_recv + MSG_HEADER_OFFSET, msg_recv_size, 5))
    {
        fprintf(stderr, "ERROR: Failed to receive message body from server.\n");
        return -1; 
    }

    // check if received message type was an error message
    char error_string[4096];
    if(msg_recv_type == MSG_TYPE_ERROR_RESPONSE)
    {
        msg_Parse_ErrorResponse(msg_recv, error_string);
        
        fprintf(stderr, "ERROR: %s\n", error_string);
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
        fprintf(stderr, "ERROR: Failed to disconnect from server %s:%d\n", server.address, server.port);
        return -1;
    }

    // success
    return 0;
}


// post message to server. returns 0 on success, -1 on error
int net_Post(ServerInfo server, char* author, char* title, char* contents)
{
    char send_msg[4096];
    char recv_msg[4096];
    
    // build message
    if(msg_Build_PostRequest(send_msg, author, title, contents))
    {
        fprintf(stderr, "ERROR: Failed to build POST REQUEST\n");
        return -1; 
    }

    // transmit message
    if(net_SendRecv(server, send_msg, recv_msg))
    {
        fprintf(stderr, "ERROR: Error occurred while sending POST REQUEST message\n");
        return -1; 
    }
 
    // success
    return 0;
}

int net_Read(ServerInfo server, int max_article_count, int* out_article_count,  Article out_articles[])
{
    char send_msg[4096];
    char recv_msg[4096];

    // build message
    if(msg_Build_ReadRequest(send_msg, max_article_count))
    {
        fprintf(stderr, "ERROR: Failed to build READ REQUEST\n");
        return -1; 
    }

    // transmit message
    if(net_SendRecv(server, send_msg, recv_msg))
    {
        fprintf(stderr, "ERROR: Error occurred while sending READ REQUEST message\n");
        return -1; 
    }
 
    // parse response
    if(msg_Parse_ReadResponse(recv_msg, out_article_count, out_articles))
    {
        fprintf(stderr, "ERROR: Error occurred while parsing READ RESPONSE message\n");
        return -1; 
    }

    // success
    return 0;
}

int net_Choose(ServerInfo server, int article_id, Article* out_data)
{
    return 0;
}

int net_Reply(ServerInfo server, int article, char* author, char* contents)
{
    return 0;
}
