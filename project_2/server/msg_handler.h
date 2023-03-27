#ifndef SERVER_MSG_HANDLER_H
#define SERVER_MSG_HANDLER_H

#include "file.h"

void msgHandler_PostRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void msgHandler_ReadRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void msgHandler_ChooseRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void msgHandler_ReplyRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void msgHandler_DbPushRequest(ServerGroup* server_group, int socket, char* msg_rcvd);
void msgHandler_DbPullRequest(ServerGroup* server_group, int socket, char* msg_rcvd);

#endif // SERVER_MSG_HANDLER_H
