#include "net.h"
#include <stdio.h>
#include "../shared/msg.h"
#include "../shared/tcp.h"

// sends message to server and receives response. return 0 on success, -1 on error
int net_SendRecv(char* address, int port, char* msg_send, char* msg_recv)
{
    int _socket = -1;
    
    uint32_t msg_send_size;
    int32_t msg_send_id;
    uint32_t msg_send_type;
    uint32_t msg_recv_size;
    int32_t msg_recv_id;
    uint32_t msg_recv_type;
    
    // connect to server
    if(tcp_Connect(address, port, &_socket))
    {
        fprintf(stderr, "ERROR: Failed to connect to server %s:%d\n", address, port);
        return -1;
    }

    // calculate message size (contents + header)
    if(msg_Parse_Header(msg_send, &msg_send_type, &msg_send_id, &msg_send_size))
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
    if(msg_Parse_Header(msg_recv, &msg_recv_type, &msg_recv_id, &msg_recv_size))
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
        fprintf(stderr, "ERROR: Failed to disconnect from server %s:%d\n", address, port);
        return -1;
    }

    // success
    return 0;
}

// post message to server. returns 0 on success, -1 on error
int net_Post(char* address, int port, char* author, char* title, char* contents)
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
    if(net_SendRecv(address, port, send_msg, recv_msg))
    {
        fprintf(stderr, "ERROR: Error occurred while sending POST REQUEST message\n");
        return -1; 
    }
 
    // success
    return 0;
}

int net_Read(char* address, int port, int max_article_count, unsigned int* out_article_count, Article out_articles[])
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
    if(net_SendRecv(address, port, send_msg, recv_msg))
    {
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

int net_Choose(char* address, int port, int article_id, Article* out_article)
{
    char send_msg[4096];
    char recv_msg[4096];

    // build message
    if(msg_Build_ChooseRequest(send_msg, article_id))
    {
        fprintf(stderr, "ERROR: Failed to build CHOOSE REQUEST\n");
        return -1;
    }

    // transmit message
    if(net_SendRecv(address, port, send_msg, recv_msg))
    {
        fprintf(stderr, "ERROR: Error occurred while sending READ REQUEST message\n");
        return -1; 
    }
 
    // parse response
    if(msg_Parse_ChooseResponse(recv_msg, out_article))
    {
        fprintf(stderr, "ERROR: Error occurred while parsing CHOOSE RESPONSE message\n");
        return -1;
    }

    // success
    return 0;
}

int net_Reply(char* address, int port, int article, char* author, char* contents)
{
    char send_msg[4096];
    char recv_msg[4096];

    // build message
    if(msg_Build_ReplyRequest(send_msg, article, author, contents))
    {
        fprintf(stderr, "ERROR: Failed to build REPLY REQUEST\n");
        return -1;
    }

    // transmit message
    if(net_SendRecv(address, port, send_msg, recv_msg))
    {
        return -1;
    }

    // success
    return 0;
}

int net_DbPush(char* address, int port, ArticleDatabase* database)
{
    char send_msg[MAX_MSG_SIZE];
    char recv_msg[4096];

    // build message
    if(msg_Build_DbPushRequest(send_msg, database))
    {
        fprintf(stderr, "ERROR: Failed to build DB PUSH REQUEST\n");
        return -1;
    }

    // transmit message
    if(net_SendRecv(address, port, send_msg, recv_msg))
    {
        return -1;
    }

    // success
    return 0;
}

int net_DbPull(char* address, int port, ArticleDatabase* out_database)
{
    char send_msg[MAX_MSG_SIZE];
    char recv_msg[MAX_MSG_SIZE];

    // build message
    if(msg_Build_DbPullRequest(send_msg))
    {
        fprintf(stderr, "ERROR: Failed to build DB PULL REQUEST\n");
        return -1;
    }

    // transmit message
    if(net_SendRecv(address, port, send_msg, recv_msg))
    {
        return -1;
    }

    // parse response
    if(msg_Parse_DbPullResponse(recv_msg, out_database))
    {
        fprintf(stderr, "ERROR: Failed to parse DB PULL RESPONSE\n");
        return -1;
    }

    // success
    return 0;
}
