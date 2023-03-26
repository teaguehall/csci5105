#ifndef SERVER_REQUEST_HANDLER_H
#define SERVER_REQUEST_HANDLER_H

#include "file.h"

void handlePostRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void handleReadRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void handleChooseRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void handleReplyRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void handleDbPushRequest(ServerGroup* server_group, int socket, char* msg_rcvd);

#endif // SERVER_REQUEST_HANDLER_H
