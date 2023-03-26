#ifndef SERVER_CONNECTION_HANDLER_H
#define SERVER_CONNECTION_HANDLER_H

#include "file.h"

typedef struct ConnectionHandlerInfo
{
    ServerGroup server_group;
    int remote_socket;

} ConnectionHandlerInfo;

void* connectionHandler(void *vargp);

#endif // SERVER_CONNECTION_HANDLER_H
