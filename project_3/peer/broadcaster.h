#ifndef PEER_BROADCASTER_H
#define PEER_BROADCASTER_H

#include "../shared/server_info.h"
#include "../shared/peer_info.h"
#include "../shared/file_info.h"

// returns 0 on success, -1 on error
int broadcaster_Init(void);
int broadcaster_GetOurFiles(int do_checksum, int* out_file_count, FileInfo out_files[]);

#endif // PEER_BROADCASTER_H
