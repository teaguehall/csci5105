#ifndef SERVER_MSG_HANDLER_H
#define SERVER_MSG_HANDLER_H

typedef struct ConnectionInfo
{
    int socket;
    char remote_address[256];
    int remote_port;

} ConnectionInfo;

void* connectionHandler(void *vargp);

#endif // SERVER_MSG_HANDLER_H
