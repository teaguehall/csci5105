#ifndef PEER_CHECKSUM_H
#define PEER_CHECKSUM_H

#include "../shared/file_info.h"

// returns 0 on success, -1 on error
int checksum_file(const char* folder, FileInfo* file_info_to_modify);
uint32_t checksum_bytes(size_t num_of_bytes, const char* bytes);

#endif // PEER_CHECKSUM_H
