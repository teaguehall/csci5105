#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "connection_handler.h"
#include "msg_handler.h"
#include "../shared/msg.h"
#include "../shared/tcp.h"
#include "../shared/database.h"
#include "../shared/net.h"
#include "../shared/tcp.h"

extern int is_coordinator;

Article articles[MAX_ARTICLES];
char response[MAX_MSG_SIZE];

void protoQuorum_Post(ServerGroup* server_group, int socket, char* author, char* title, char* contents)
{
    int i, db_full;
    int success = 0;
    int from_server = 1;
    char remote_addr[256];
    int remote_port;

    // get ip address and port of remote host (to see who sent the request, client or server)
    if(tcp_GetRemoteAddr(socket, remote_addr, &remote_port))
    {
        fprintf(stderr, "ERROR: Failed to get remote address and port of socket\n");
        return;
    }

    // check if the connection is from another server in the group
    for(i = 0; i < (server_group->server_count); i++)
    {
        if(strcmp(server_group->servers[i].address, remote_addr) == 0 && server_group->servers[i].port == remote_port)
        {
            from_server = 1;
        }
    }

    // write article to Nw servers (unless message was originated from another group server)
    if(!from_server)
    {
        for(i = 0; i < (server_group->server_count); i++)
        {            
            if(net_Post(server_group->servers[i].address, server_group->servers[i].port, author, title, contents) == 0)
            {
                success++;
            }
                    
            // break out if enough messages sent
            if(success >= server_group->nw)
                break;
        }
    }
    else
    {
        success = server_group->nw;
        db_Post(author, title, contents, &db_full);
    }

    // build response
    if(success >= server_group->nw)
    {
        msg_Build_PostResponse(response);
    }
    else
    {
        msg_Build_ErrorResponse(response, "Failed to write to Nw servers...");
    }

    // send response
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send CHOOSE-RESPONSE\n");
    }
}

void protoQuorum_Read(ServerGroup* server_group, int socket)
{
    ArticleDatabase temp_db;
    ArticleDatabase saved_db;
    int article_count;
    Article articles[MAX_ARTICLES];

    saved_db.version = -1;

    int success = 0;

    // attempt to read from to Nr servers
    for(int i = 0; i < server_group->server_count; i++)
    {
        // read from primary server first
        if(net_DbPull(server_group->servers[i].address, server_group->servers[i].port, &temp_db) == 0)
                success++;

        // save database if it's the highest version
        if(temp_db.version > saved_db.version)
        {
            memcpy(&saved_db, &temp_db, sizeof(ArticleDatabase));
        }
        
        // break out if enough messages sent
        if(success >= server_group->nr)
            break;
    }

    // build response
    if(success >= server_group->nr)
    {
        db_Read(&article_count, articles);
        msg_Build_ReadResponse(response, article_count, articles);
    }
    else
    {
        msg_Build_ErrorResponse(response, "Failed to read from Nr servers...");
    }

    // send response
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send READ-RESPONSE\n");
    }
}

void protoQuorum_Reply(ServerGroup* server_group, int socket, int article_id, char* author, char* contents)
{
    int i, db_full, invalid_article;
    int success = 0;
    int from_server = 1;
    char remote_addr[256];
    int remote_port;

    // get ip address and port of remote host (to see who sent the request, client or server)
    if(tcp_GetRemoteAddr(socket, remote_addr, &remote_port))
    {
        fprintf(stderr, "ERROR: Failed to get remote address and port of socket\n");
        return;
    }

    // check if the connection is from another server in the group
    for(i = 0; i < (server_group->server_count); i++)
    {
        if(strcmp(server_group->servers[i].address, remote_addr) == 0 && server_group->servers[i].port == remote_port)
        {
            from_server = 1;
        }
    }

    // write article to Nw servers (unless message was originated from another group server)
    if(!from_server)
    {
        for(i = 0; i < (server_group->server_count); i++)
        {
            printf("i = %d, server count = %d\n", i, server_group->server_count); // TODO delete
            
            if(net_Reply(server_group->servers[i].address, server_group->servers[i].port, article_id, author, contents) == 0)
            {
                success++;
            }
                    
            // break out if enough messages sent
            if(success >= server_group->nw)
                break;
        }
    }
    else
    {
        success = server_group->nw;
        db_Reply(article_id, author, contents, &db_full, &invalid_article);
        if(invalid_article)
        {
            msg_Build_ErrorResponse(response, "Attempted to reply to invalid article...");
        }
    }

    // build response
    if(success >= server_group->nw)
    {
        msg_Build_ReplyResponse(response);
    }
    else
    {
        msg_Build_ErrorResponse(response, "Failed to write to Nw servers...");
    }

    // send response
    if(tcp_Send(socket, response, msg_GetActualSize(response), 5))
    {
        fprintf(stderr, "ERROR: Failed to send CHOOSE-RESPONSE\n");
    }
}

void* quorumSyncThread(void *vargp)
{
    ServerGroup server_group;
    ArticleDatabase local_db;
    ArticleDatabase rcvd_db;

    // cast/copy arguments to local variables
    server_group = ((ConnectionHandlerInfo*)(vargp))->server_group;

    // monitor once a second
    while(1)
    {
        // iterate through servers to check for new version
        for(int i = 0; i < server_group.server_count; i++)
        {
            db_Backup(&local_db);
            net_DbPull(server_group.servers[i].address, server_group.servers[i].port, &rcvd_db);
            if(rcvd_db.version > local_db.version)
            {
                db_Restore(&rcvd_db);
            }

            sleep(1);
        }
    }
}