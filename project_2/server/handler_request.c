#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "handler_request.h"
#include "../shared/msg.h"
#include "../shared/tcp.h"
#include "../shared/database.h"
#include "../shared/net.h"

extern int is_coordinator;

Article articles[MAX_ARTICLES];
char response[MAX_ARTICLES * sizeof(Article) + MSG_HEADER_OFFSET];      // make response buffer big enough to accomadate largest scenario

// handles post request message from clients
void handlePostRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{   
    ArticleDatabase db_snapshot;
    char error_msg[4096];
    int db_full;
    
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

    // sequential consistency
    if(server_group->protocol == PROTOCOL_SEQUENTIAL)
    {
        // if we're the coordinator, update our database and then push update to all other replicas
        if(is_coordinator)
        {
            // post to database
            if(db_Post(author, title, contents, &db_full) == -1)
            {
                msg_Build_ErrorResponse(response, "Primary database is full...");
                goto send_response;
            }

            // take snapshot of database (will send this to all replicas)
            db_Backup(&db_snapshot);

            printf("db snapshot version = %d\n", db_snapshot.version);

            // push updated database to all other replicas
            for(int i = 0; i < server_group->server_count - 1; i++)
            {
                if(net_DbPush(server_group->others[i].address, server_group->others[i].port, &db_snapshot))
                {
                    sprintf(error_msg, "Failed to replicate database to %s:%d. Server down?", server_group->others[i].address, server_group->others[i].port);
                    msg_Build_ErrorResponse(response, error_msg);
                    goto send_response;
                }
            }
        }
        else // if we're NOT the coordinator, forward message to the coordinator
        {
            if(net_Post(server_group->primary.address, server_group->primary.port, author, title, contents))
            {
                msg_Build_ErrorResponse(response, "Post request failed to commit to coordinator server...");
            }
        }
    }

    // build success response
    msg_Build_PostResponse(response);

    // send response
    send_response:
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
    }
}

// handles read request message from clients
void handleReadRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{
    int count;
    
    // parsed reponse
    uint32_t max_articles;

    // extract message contents
    if(msg_Parse_ReadRequest(msg_rcvd, &max_articles))
    {
        printf("ERROR while parsing READ REQUEST message");
        return;
    }

    // read articles from database
    db_Read(&count, articles);

    // build response
    msg_Build_ReadResponse(response, count, articles);
    
    // send response
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send CHOOSE-RESPONSE\n");
    }
}

// handles choose request message from clients
void handleChooseRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
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

    // TODO - implement consistency!!!
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
void handleReplyRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
{
    ArticleDatabase db_snapshot;
    char error_msg[4096];

    int db_full, invalid_id;

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

    // sequential consistency
    if(server_group->protocol == PROTOCOL_SEQUENTIAL)
    {
        // if we're the coordinator, update our database and then push update to all other replicas
        if(is_coordinator)
        {
            // submit reply to database
            if(db_Reply(response_id, author, contents, &db_full, &invalid_id) == -1)
            {
                if(db_full)
                {
                    msg_Build_ErrorResponse(response, "Primary database is full...");
                    goto send_response;
                }
                else if(invalid_id)
                {
                    sprintf(error_msg, "Client attempted to reply to article \"%u\" which does not exist", response_id);
                    msg_Build_ErrorResponse(response, error_msg);
                    goto send_response;
                }
                else
                {
                    msg_Build_ErrorResponse(response, "Failed to submit reply");
                    goto send_response;
                }
            }

            // take snapshot of database (will send this to all replicas)
            db_Backup(&db_snapshot);

            printf("db snapshot version = %d\n", db_snapshot.version);

            // push updated database to all other replicas
            for(int i = 0; i < server_group->server_count - 1; i++)
            {
                if(net_DbPush(server_group->others[i].address, server_group->others[i].port, &db_snapshot))
                {
                    sprintf(error_msg, "Failed to replicate database to %s:%d. Server down?", server_group->others[i].address, server_group->others[i].port);
                    msg_Build_ErrorResponse(response, error_msg);
                    goto send_response;
                }
            }
        }
        else // if we're NOT the coordinator, forward message to the coordinator
        {
            if(net_Reply(server_group->primary.address, server_group->primary.port, response_id, author, contents))
            {
                msg_Build_ErrorResponse(response, "Reply request failed to commit to coordinator server...");
                goto send_response;
            }
        }
    }

    // build success response
    msg_Build_PostResponse(response);    

    // send response
    send_response:
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
    }
}

// handles db push requeset from servers
void handleDbPushRequest(ServerGroup* server_group, int socket, char* msg_rcvd)
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

    printf("Server received replicated database! db_rcvd.count = %d\n", db_rcvd.article_count);
}
