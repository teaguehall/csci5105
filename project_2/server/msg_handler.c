#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "msg_handler.h"
#include "../shared/msg.h"
#include "../shared/tcp.h"
#include "../shared/database.h"
#include "../shared/net.h"

#include "protocol.h"


extern int is_coordinator;

Article articles[MAX_ARTICLES];
char response[MAX_ARTICLES * sizeof(Article) + MSG_HEADER_OFFSET];      // make response buffer big enough to accomadate largest scenario

// handles post request message from clients
void msgHandler_PostRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{   
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

    // handle message per current protocol
    switch(server_group->protocol)
    {
        case PROTOCOL_SEQUENTIAL:
            protoSequential_Post(server_group, socket, author, title, contents);
            break;
        case PROTOCOL_QUORUM:
            protoQuorum_Post(server_group, socket, author, title, contents);
            break;
        case PROTOCOL_READ_YOUR_WRITE:
            protoReadYourWrite_Post(server_group, socket, author, title, contents);
            break; 
        default: // should never get here
            break;
    }
}

// handles read request message from clients
void msgHandler_ReadRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{    
    // parsed reponse
    uint32_t max_articles;

    // extract message contents
    if(msg_Parse_ReadRequest(msg_rcvd, &max_articles))
    {
        printf("ERROR while parsing READ REQUEST message");
        return;
    }

    // handle message per current protocol
    switch(server_group->protocol)
    {
        case PROTOCOL_SEQUENTIAL:
            protoSequential_Read(server_group, socket);
            break;
        case PROTOCOL_QUORUM:
            protoQuorum_Read(server_group, socket);
            break;
        case PROTOCOL_READ_YOUR_WRITE:
            protoReadYourWrite_Read(server_group, socket);
            break; 
        default: // should never get here
            break;
    }
}

// handles choose request message from clients
void msgHandler_ChooseRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{    
    char error_msg[4096];
    
    Article article;
    int invalid_id;
    
    // parsed reponse
    uint32_t article_id;

    // extract message contents
    if(msg_Parse_ChooseRequest(msg_rcvd, &article_id))
    {
        printf("ERROR while parsing CHOOSE REQUEST message");
        return;
    }

    // choose article from database
    if(db_Choose(article_id, &article, &invalid_id) == -1)
    {
        if(invalid_id)
        {
            sprintf(error_msg, "Client attempted to reply to article \"%u\" which does not exist", article_id);
            msg_Build_ErrorResponse(response, error_msg);
        }
        else
        {
            msg_Build_ErrorResponse(response, "Failed to submit reply");
        }
    }
    else
    {
        msg_Build_ChooseResponse(response, article);
    }
    
    // send response
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send CHOOSE-RESPONSE\n");
    }   
}

// handles reply request message from clients
void msgHandler_ReplyRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{
    // parsed reponse
    uint32_t response_id;
    char author[4096];
    char contents[4096];

    // extract message contents
    if(msg_Parse_ReplyRequest(msg_rcvd, &response_id, author, contents))
    {
        printf("ERROR while parsing Post request message");
        return;
    }

    // handle message per current protocol
    switch(server_group->protocol)
    {
        case PROTOCOL_SEQUENTIAL:
            protoSequential_Reply(server_group, socket, response_id, author, contents);
            break;
        case PROTOCOL_QUORUM:
            protoQuorum_Reply(server_group, socket, response_id, author, contents);
            break;
        case PROTOCOL_READ_YOUR_WRITE:
            protoReadYourWrite_Reply(server_group, socket, response_id, author, contents);
            break; 
        default: // should never get here
            break;
    }
}

// handles db push request from servers
void msgHandler_DbPushRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{
    ArticleDatabase db_rcvd;

    // extract database from message
    if(msg_Parse_DbPushRequest(msg_rcvd, &db_rcvd))
    {
        printf("ERROR while parsing Post request message");
        return;
    }

    // restore rcvd db to our servers db instance
    db_Restore(&db_rcvd);

    // build response
    msg_Build_DbPushResponse(response);

    // send response
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
    }
}

// handles db pull request from servers
void msgHandler_DbPullRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{
    ArticleDatabase db_snapshot;

    // take snapshot of our database
    db_Backup(&db_snapshot);

    // build response
    msg_Build_DbPullResponse(response, &db_snapshot);

    // send response
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
    }
}

