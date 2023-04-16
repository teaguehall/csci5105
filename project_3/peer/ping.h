#ifndef PEER_PING_H
#define PEER_PING_H

#include "../shared/peer_info.h"

// returns 0 on success, -1 on error
int ping_Init(PeerInfo* our_info);

#endif // PEER_PING_H
