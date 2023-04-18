#ifndef PEER_CHECKSUM_H
#define PEER_CHECKSUM_H

#include "../shared/file_info.h"

// returns 0 on success, -1 on error
int checksum(FileInfo* file_info_to_modify);

#endif // PEER_CHECKSUM_H
