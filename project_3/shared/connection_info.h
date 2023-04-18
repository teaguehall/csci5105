#ifndef SHARED_CONNECTION_INFO_H
#define SHARED_CONNECTION_INFO_H

typedef struct ConnectionInfo
{
    int socket;
    char remote_addr[256];
    int remote_port;

} ConnectionInfo;

#endif // SHARED_CONNECTION_INFO_H
