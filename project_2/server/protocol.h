#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "file.h"

void* quorumSyncThread(void *vargp);

// sequential stuff
void protoSequential_Post(ServerGroup* server_group, int socket, char* author, char* title, char* contents);
void protoSequential_Read(ServerGroup* server_group, int socket);
void protoSequential_Reply(ServerGroup* server_group, int socket, int article_id, char* author, char* contents);

// quorum stuff
void protoQuorum_Post(ServerGroup* server_group, int socket, char* author, char* title, char* contents);
void protoQuorum_Read(ServerGroup* server_group, int socket);
void protoQuorum_Reply(ServerGroup* server_group, int socket, int article_id, char* author, char* contents);

// read your write stuff
void protoReadYourWrite_Post(ServerGroup* server_group, int socket, char* author, char* title, char* contents);
void protoReadYourWrite_Read(ServerGroup* server_group, int socket);
void protoReadYourWrite_Reply(ServerGroup* server_group, int socket, int article_id, char* author, char* contents);

#endif // SERVER_PROTOCOL_H