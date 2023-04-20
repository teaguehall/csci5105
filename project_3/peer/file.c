#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "file.h"
#include "checksum.h"

int file_writer(const char* folder_name, const FileInfo* file_info, const char* data)
{
    char file_path[512];

    // build full file path
    strcpy(file_path, folder_name);
    strcat(file_path, "/");
    strcat(file_path, file_info->name);    
    
    int error = 0; // assume success

    // create file
    FILE* file = fopen(file_path, "w" );
    if(file == NULL)
    {
        fprintf(stderr, "ERROR: Failed to write downloaded file \"%s\" to disk: %s\n", file_path, strerror(errno));
        return -1;
    }

    // write bytes to file
    if(fwrite(data, 1, file_info->size, file) != file_info->size)
    {
        fprintf(stderr, "ERROR: Failed to write downloaded file \"%s\" to disk: %s\n",file_path, strerror(errno));
        error = -1;
    }

    // close file and exit
    fclose(file);
    return error;
}

int file_reader(const char* folder_name, const char* file_name, FileInfo* out_file_info, char* out_data)
{
    char file_path[512];

    // build full file path
    strcpy(file_path, folder_name);
    strcat(file_path, "/");
    strcat(file_path, file_name);
    
    int error = 0; // assume success   
    
    // calculate file info data
    strcpy(out_file_info->name, file_name);
    if(checksum_file(folder_name, out_file_info))
    {
        fprintf(stderr, "ERROR: Failed to open file \"%s\"\n", file_name);
        return -1;
    }
    
    // open file
    FILE* file = fopen(file_path, "r");
    if(file == NULL)
    {
        fprintf(stderr, "ERROR: Failed to open file \"%s\" on disk: %s\n", file_name, strerror(errno));
        return -1;
    }

    // read bytes from file
    if(fread(out_data, 1, out_file_info->size, file) != out_file_info->size)
    {
        fprintf(stderr, "ERROR: Failed to write downloaded file \"%s\" to disk: %s\n", out_file_info->name, strerror(errno));
        error = -1;
    }

    // close file and exit
    fclose(file);
    return error;
}
