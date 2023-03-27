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

extern int is_coordinator;

Article articles[MAX_ARTICLES];
char response[MAX_MSG_SIZE];

// handles post request message from clients
void protoSequential_Post(ServerGroup* server_group, int socket, char* author, char* title, char* contents)
{   
    ArticleDatabase db_snapshot;
    int db_full;

    // if we're the coordinator, update our database and then push update to all other replicas
    if(is_coordinator)
    {
        // post to database
        if(db_Post(author, title, contents, &db_full) == -1)
        {
            msg_Build_ErrorResponse(response, "Primary database is full...");
            if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
            {
                fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
            }
            return;
        }

        // build success response
        msg_Build_PostResponse(response);
        if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
        {
            fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
        }

        // take snapshot of database (will send this to all replicas)
        db_Backup(&db_snapshot);

        // push updated database to all other replicas
        for(int i = 0; i < server_group->server_count - 1; i++)
        {
            net_DbPush(server_group->servers[i].address, server_group->servers[i].port, &db_snapshot);
        }
    }
    else // if we're NOT the coordinator, forward message to the coordinator
    {
        if(net_Post(server_group->servers[0].address, server_group->servers[0].port, author, title, contents))
        {
            // build error response
            msg_Build_ErrorResponse(response, "Post request failed to commit to coordinator server...");
            if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
            {
                fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
            }
        }
        else
        {
            // build success response
            msg_Build_PostResponse(response);
            if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
            {
                fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
            }
        }
    }
}

// handles read request message from clients
void protoSequential_Read(ServerGroup* server_group, int socket)
{
    int count;

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

// handles reply request message from clients
void protoSequential_Reply(ServerGroup* server_group, int socket, int article_id, char* author, char* contents)
{
    ArticleDatabase db_snapshot;
    char error_msg[4096];

    int db_full, invalid_id;

    // if we're the coordinator, update our database and then push update to all other replicas
    if(is_coordinator)
    {
        // submit reply to database
        if(db_Reply(article_id, author, contents, &db_full, &invalid_id) == -1)
        {           
            if(db_full)
            {
                msg_Build_ErrorResponse(response, "Primary database is full...");
                goto send_err_response;
            }
            else if(invalid_id)
            {
                sprintf(error_msg, "Client attempted to reply to article \"%u\" which does not exist", article_id);
                msg_Build_ErrorResponse(response, error_msg);
                goto send_err_response;
            }
            else
            {
                msg_Build_ErrorResponse(response, "Failed to submit reply");
                goto send_err_response;
            }
        }

        // send success response prior to replicating data to other databases
        msg_Build_ReplyResponse(response);    
        if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
        {
            fprintf(stderr, "ERROR: Failed to send REPLY RESPONSE\n");
        }

        // take snapshot of database (will send this to all replicas)
        db_Backup(&db_snapshot);    
        
        // push updated database to all other replicas
        for(int i = 0; i < server_group->server_count - 1; i++)
        {
            if(net_DbPush(server_group->servers[i].address, server_group->servers[i].port, &db_snapshot))
            {
                sprintf(error_msg, "Failed to replicate database to %s:%d. Server down?", server_group->servers[i].address, server_group->servers[i].port);
                msg_Build_ErrorResponse(response, error_msg);
                goto send_err_response;
            }
        }
    }
    else // if we're NOT the coordinator, forward message to the coordinator
    {
        if(net_Reply(server_group->servers[0].address, server_group->servers[0].port, article_id, author, contents))
        {
            msg_Build_ErrorResponse(response, "Reply request failed to commit to coordinator server...");
            goto send_err_response;
        }

        // send success response
        msg_Build_ReplyResponse(response);    
        if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
        {
            fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
        }
    }

    // success
    return;

    // send error response
    send_err_response:
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send POST RESPONSE\n");
    }
}
