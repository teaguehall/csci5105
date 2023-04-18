#ifndef PEER_INFO_H
#define PEER_INFO_H

typedef struct PeerInfo
{
    char listening_addr[256];
    int listening_port;

    double latitude;
    double longitude;

} PeerInfo;


#endif // PEER_INFO_H
