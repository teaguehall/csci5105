#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "file_writer.h"

void file_writer(const FileInfo* file_info, const char* data)
{
    // create file
    FILE* file = fopen(file_info->name, "w" );
    if(file == NULL)
    {
        fprintf(stderr, "ERROR: Failed to write downloaded file \"%s\" to disk: %s\n", file_info->name, strerror(errno));
        return;
    }

    // write bytes to file
    if(fwrite(data, 1, file_info->size, file) != file_info->size)
    {
        fprintf(stderr, "ERROR: Failed to write downloaded file \"%s\" to disk: %s\n", file_info->name, strerror(errno));
        return;
    }

    // close file and exit
    fclose(file);
    return;
}
