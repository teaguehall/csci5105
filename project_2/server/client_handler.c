#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "client_handler.h"
#include "../shared/msg.h"
#include "../shared/tcp.h"

// handles post request message from clients
static void handlePostRequest(int socket, char* msg_rcvd)
{
    char response[8192];
    
    // parsed reponse
    char author[4096];
    char title[4096];
    char contents[4096]; 

    // extract message contents
    if(msg_Parse_PostRequest(msg_rcvd, author, title, contents))
    {
        printf("ERROR while parsing Post request message");
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// TODO do something /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    printf("Server received post: author = %s, title = %s, contents = %s\n", author, title, contents); 

    // build response
    msg_Build_PostResponse(response);

    // send response
    if(tcp_Send(socket, response, MSG_HEADER_OFFSET, 5))
    {
        fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
    }
}

// handles read request message from clients
static void handleReadRequest(int socket, char* msg_rcvd)
{
    char response[8192];
    
    // parsed reponse
    uint32_t page_size;
    uint32_t page_number;

    // extract message contents
    if(msg_Parse_ReadRequest(msg_rcvd, &page_size, &page_number))
    {
        printf("ERROR while parsing READ REQUEST message");
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// TODO do something /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    printf("Server received READ REQUEST: page size = %u, page number = %u\n", page_size, page_number);
    Article articles[2];

    articles[0].id = 1;
    articles[0].parent_id = 0;
    articles[0].depth = 0;
    strcpy(articles[0].author, "Teague");
    strcpy(articles[0].title, "Help Wanted!");
    strcpy(articles[0].contents, "I need help doing electrical!");

    articles[1].id = 2;
    articles[1].parent_id = 1;
    articles[1].depth = 1;
    strcpy(articles[1].author, "Adam");
    strcpy(articles[1].title, "RE: Help Wanted!");
    strcpy(articles[1].contents, "What kind of electrical do you need?");

    // build response
    msg_Build_ReadResponse(response, 2, articles);

    // find message size
    uint32_t msg_type;
    uint32_t msg_size;

    msg_Parse_Header(response, &msg_type, &msg_size);
    
    // send response
    if(tcp_Send(socket, response, msg_size + MSG_HEADER_OFFSET, 5))
    {
        fprintf(stderr, "ERROR: Failed to send CHOOSE-RESPONSE\n");
    }      
}

// handles choose request message from clients
static void handleChooseRequest(int socket, char* msg_rcvd)
{
    char response[8192];
    
    // parsed reponse
    uint32_t article_id;

    // extract message contents
    if(msg_Parse_ChooseRequest(msg_rcvd, &article_id))
    {
        printf("ERROR while parsing CHOOSE REQUEST message");
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// TODO do something /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    printf("Server received CHOOSE REQUEST: article id = %u\n", article_id);
    Article article;

    article.id = 1;
    article.parent_id = 0;
    article.depth = 0;
    strcpy(article.author, "Teague");
    strcpy(article.title, "Okay then!");
    strcpy(article.contents, "You requested this!");

    // build response
    msg_Build_ChooseResponse(response, article);

    // find message size
    uint32_t msg_type;
    uint32_t msg_size;

    msg_Parse_Header(response, &msg_type, &msg_size);
    
    // send response
    if(tcp_Send(socket, response, msg_size + MSG_HEADER_OFFSET, 5))
    {
        fprintf(stderr, "ERROR: Failed to send CHOOSE-RESPONSE\n");
    }   
}

// handles reply request message from clients
static void handleReplyRequest(int socket, char* msg_rcvd)
{
    char response[8192];
    
    // parsed reponse
    uint32_t article_id;

    // extract message contents
    if(msg_Parse_ChooseRequest(msg_rcvd, &article_id))
    {
        printf("ERROR while parsing CHOOSE REQUEST message");
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// TODO do something /////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////
    printf("Server received CHOOSE REQUEST: article id = %u\n", article_id);
    Article article;

    article.id = 1;
    article.parent_id = 0;
    article.depth = 0;
    strcpy(article.author, "Teague");
    strcpy(article.title, "Okay then!");
    strcpy(article.contents, "You requested this!");

    // build response
    msg_Build_ChooseResponse(response, article);

    // find message size
    uint32_t msg_type;
    uint32_t msg_size;

    msg_Parse_Header(response, &msg_type, &msg_size);
    
    // send response
    if(tcp_Send(socket, response, msg_size + MSG_HEADER_OFFSET, 5))
    {
        fprintf(stderr, "ERROR: Failed to send CHOOSE-RESPONSE\n");
    }   
}

void* funcClientHandler(void *vargp)
{
    uint32_t msg_recv_type;
    uint32_t msg_recv_size;
    
    int listener_socket = -1;
    int remote_socket = -1;
    char remote_addr[128];
    int remote_port;

    char recv_msg[4096];
    
    // cast input args
    char* listening_addr = ((ClientHandlerInfo*)(vargp))->listening_address;
    int listening_port = ((ClientHandlerInfo*)(vargp))->listening_port;  
    
    // create listener socket
    if(tcp_CreateListener(listening_addr, listening_port, &listener_socket))
    {
        printf("Failed to create listener socket\n");
        exit(EXIT_FAILURE);
    }
    
    // accept connections
    while(1)
    {        
        // accept connection
        if(tcp_Accept(listener_socket, remote_addr, &remote_port, &remote_socket))
        {
            printf("ERROR occurred while accepting connection\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // wait for header
        if(tcp_Recv(remote_socket, recv_msg, MSG_HEADER_OFFSET, 5))
        {
            printf("ERROR occurred while receiving message header\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // extract message header info
        if(msg_Parse_Header(recv_msg, &msg_recv_type, &msg_recv_size))
        {
            printf("Server received invalid header from client\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // read rest of message
        if(tcp_Recv(remote_socket, recv_msg + MSG_HEADER_OFFSET, msg_recv_size, 5))
        {
            printf("ERROR occurred while receiving message body\n");
            tcp_Disconnect(remote_socket);
            continue;
        }

        // handle rest of message depending on type
        switch(msg_recv_type)
        {
            case MSG_TYPE_POST_REQUEST :
                handlePostRequest(remote_socket, recv_msg);
                break;
            case MSG_TYPE_READ_REQUEST :
                handleReadRequest(remote_socket, recv_msg);
                break;
            case MSG_TYPE_CHOOSE_REQUEST :
                handleChooseRequest(remote_socket, recv_msg);
                break;
            case MSG_TYPE_REPLY_REQUEST :
                handleReplyRequest(remote_socket, recv_msg);
                break;
            default:
                fprintf(stderr, "ERROR: Server received unrecognized message\n");
                continue;
        }

        // disconnect from client
        tcp_Disconnect(remote_socket);

    }

    // success
    return NULL;
}

