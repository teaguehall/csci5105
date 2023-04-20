#ifndef PEER_FILE_WRITER_H
#define PEER_FILE_WRITER_H

#include "../shared/file_info.h"

// returns -1 on error, 0 on success
int file_writer(const FileInfo* file_info, const char* data);

// returns -1 on error, 0 on success
int file_reader(const char* folder_name, const char* file_name, FileInfo* out_file_info, char* out_data);

#endif // PEER_FILE_WRITER_H
