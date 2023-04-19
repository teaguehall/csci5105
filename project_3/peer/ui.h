#ifndef PEER_UI_H
#define PEER_UI_H

#include "../shared/server_info.h"
#include "../shared/peer_info.h"

// returns 0 on success, -1 on error
int ui_Init(ServerInfo* server_info, PeerInfo* peer_info, const char* shared_dir);

#endif // PEER_UI_H
