#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <stdint.h>

#define MAX_FILE_NAME_SIZE 512      // bytes

typedef struct FileInfo
{
    char name[MAX_FILE_NAME_SIZE];
    uint32_t size;                  // size of file in bytes
    uint32_t check_sum;             // resulting checksum of file data

} FileInfo;


#endif // FILE_INFO_H
