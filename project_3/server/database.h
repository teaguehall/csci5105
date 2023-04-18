#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include "../shared/peer_info.h"
#include "../shared/file_info.h"

int db_Init(void);
int db_AddPeer(const PeerInfo* peer, int num_of_files, const FileInfo files[]);
int db_KeepAlive(const PeerInfo* peer);
int db_FindFile(const char* file_name, int* out_num_of_peers, PeerInfo out_peers[]);
int db_DiscoverFiles(int* out_num_of_files, FileInfo out_files[]);

#endif // SERVER_DATABASE_H
