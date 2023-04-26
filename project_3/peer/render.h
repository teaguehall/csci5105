#ifndef PEER_RENDER_H
#define PEER_RENDER_H

#include "../shared/file_info.h"
#include "../shared/peer_info.h"

#define DIVIDER_LINE "---------------------------------------------------------------------------\n"

void render_ClearTerminal(void);
void render_HelpMenu(void);
void render_OurFiles(int num_of_files, const FileInfo files[]);
void render_DiscoveredFiles(int num_of_files, const FileInfo files[]);
void render_PeersWithFile(const char* file_name, int num_of_peers, const PeerInfo peers[]);

#endif // PEER_RENDER_H
