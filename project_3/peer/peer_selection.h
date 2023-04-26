#ifndef PEER_SELECTION_H
#define PEER_SELECTION_H

#include "../shared/peer_info.h"
#include "../shared/file_info.h"

void peerSelection_Sort(int bytes_to_download, int num_of_peers, PeerInfo peers[]);

#endif // PEER_SELECTION_H
