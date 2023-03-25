#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "file.h"

// parses file to fill out server group struct. 
int file_ParseServerGroup(char* file_path, ServerGroup* out_group)
{
    char line[1024];
    int error = 0;

    FILE* file;

    // open file
    file = fopen(file_path, "r");
    if(file == NULL)
    {
        fprintf(stderr, "ERROR: Failed to parse file \"%s\". %s\n", file_path, strerror(errno));
        return -1;
    }

    // count number of lines to determine total number of servers

    // grab each line
    while(fgets(line, sizeof(line), file) != NULL)
    {
        printf("%s\n", line);
    }

    // close file
    //exit:
    fclose(file);

    // return error status
    return error;

}