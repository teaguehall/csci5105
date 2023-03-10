#ifndef SERVER_CLIENT_HANDLER_H
#define SERVER_CLIENT_HANDLER_H

typedef struct ClientHandlerInfo
{
    char listening_address[128];
    int listening_port;

} ClientHandlerInfo;

void* client_Handler(void *vargp);

#endif // SERVER_CLIENT_HANDLER_H