#ifndef PEER_BROADCASTER_H
#define PEER_BROADCASTER_H

#include "../shared/server_info.h"
#include "../shared/peer_info.h"

// returns 0 on success, -1 on error
int broadcaster_Init(ServerInfo* server_info, PeerInfo* peer_info, const char* shared_dir);

#endif // PEER_BROADCASTER_H
