#ifndef SERVER_CONNECTION_HANDLER_H
#define SERVER_CONNECTION_HANDLER_H

typedef struct ConnectionHandlerInfo
{
    int remote_socket;
    char remote_addr[128];
    int remote_port;

} ConnectionHandlerInfo;

void* connectionHandler(void *vargp);

#endif // SERVER_CONNECTION_HANDLER_H
