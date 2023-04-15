#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <stdint.h>

typedef struct FileInfo
{
    char name[512];
    uint32_t bytes;

} FileInfo;


#endif // FILE_INFO_H
