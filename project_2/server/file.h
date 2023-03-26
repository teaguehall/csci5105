#ifndef SERVER_FILE_H
#define SERVER_FILE_H

#define MAX_GROUP_SIZE                  1024    // number of servers

#define PROTOCOL_SEQUENTIAL             1
#define PROTOCOL_QUORUM                 2
#define PROTOCOL_READ_YOUR_WRITE        3

typedef struct Server
{
    char address[256];
    int port;
} Server;

typedef struct ServerGroup
{
    int protocol;
    int nr; // only used when quorum protocol selected
    int nw; // only used when quorum protocol selected
    int server_count;
    Server primary;
    Server others[MAX_GROUP_SIZE];
} ServerGroup;

int file_ParseServerGroup(char* file_path, ServerGroup* out_group);

#endif // SERVER_FILE_H
