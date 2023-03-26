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
    printf("TODO\n");
}

void protoQuorum_Read(ServerGroup* server_group, int socket)
{
    printf("TODO\n");
}

void protoQuorum_Choose(ServerGroup* server_group, int socket, int article_id)
{
    printf("TODO\n");
}

void protoQuorum_Reply(ServerGroup* server_group, int socket, int article_id, char* author, char* contents)
{
    printf("TODO\n");
}