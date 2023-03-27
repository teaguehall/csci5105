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

void protoQuorum_Post(ServerGroup* server_group, int socket, char* author, char* title, char* contents)
{
    int success = 0;
    
    // attempt to write article to Nw servers
    for(int i = 0; i < server_group->server_count; i++)
    {
        // send to primary server first
        if(i == 0)
        {
            if(net_Post(server_group->primary.address, server_group->primary.port, author, title, contents) == 0)
                success++;
        }
        else // then move on to all others
        {
            if(net_Post(server_group->others[i-1].address, server_group->others[i-1].port, author, title, contents) == 0)
                success++;
        }
        
        // break out if enough messages sent
        if(success >= server_group->nw)
            break;

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
    printf("TODO\n");
}

void protoQuorum_Reply(ServerGroup* server_group, int socket, int article_id, char* author, char* contents)
{
    printf("TODO\n");
}