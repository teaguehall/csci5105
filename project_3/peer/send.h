#ifndef CLIENT_SEND_H
#define CLIENT_SEND_H

#include <stdlib.h>

#include "../shared/file_info.h"
#include "../shared/peer_info.h"
#include "../shared/server_info.h"

int send_PingRequest(const ServerInfo* server, const PeerInfo* us, int* out_recognized);
int send_UpdateListRequest(const ServerInfo* server, const PeerInfo* us, size_t file_count, const FileInfo files[]);
int send_DiscoverRequest(const ServerInfo* server, int* out_file_count, FileInfo out_files[]);
int send_FindRequest(const ServerInfo* server, const char* file_name, int* out_peer_count, PeerInfo out_peers[]);
int send_GetLoadThoughputRequest(const PeerInfo* peer, int* bytes_per_sec);
int send_DownloadRequest(const PeerInfo* peer, const char* file_name, FileInfo* out_file_info, char* out_file_data);

#endif // CLIENT_SEND_H
