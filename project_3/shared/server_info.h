#ifndef SERVER_INFO_H
#define SERVER_INFO_H

typedef struct ServerInfo
{
    char listening_addr[256];
    int listening_port;

} ServerInfo;

#endif // SERVER_INFO_H
