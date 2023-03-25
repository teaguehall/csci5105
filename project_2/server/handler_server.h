#ifndef SERVER_SERVER_HANDLER_H
#define SERVER_SERVER_HANDLER_H

typedef struct ClientHandlerInfo
{
    char listening_address[128];
    int listening_port;

} ClientHandlerInfo;

void* funcClientHandler(void *vargp);

#endif // SERVER_SERVER_HANDLER_H
