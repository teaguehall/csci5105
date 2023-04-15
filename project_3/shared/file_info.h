#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <stdint.h>

typedef struct FileInfo
{
    char name[512];
    uint32_t size;          // size of file in bytes
    uint32_t check_sum;     // resulting checksum of file data

} FileInfo;


#endif // FILE_INFO_H
