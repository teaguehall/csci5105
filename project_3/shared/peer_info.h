#ifndef PEER_INFO_H
#define PEER_INFO_H

typedef struct PeerInfo
{
    char address[256];
    int port;

    double latitude;
    double longitude;

} PeerInfo;


#endif // PEER_INFO_H
