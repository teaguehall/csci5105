// this entire function is trash but is good enough

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "file.h"
#include "../shared/tcp.h"

// parses file to fill out server group struct. 
int file_ParseServerGroup(char* file_path, ServerGroup* out_group)
{
    char line[1024];
    char str_address[1024];
    char str_port[1024];
    int error = 0;
    int colon_found, colon_pos;

    FILE* file;

    // open file
    file = fopen(file_path, "r");
    if(file == NULL)
    {
        fprintf(stderr, "ERROR: Failed to parse file \"%s\". %s\n", file_path, strerror(errno));
        return -1;
    }

    // grab first line (protocol)
    if(fgets(line, sizeof(line), file) != NULL)
    {
        // trim new line char
        line[strlen(line) - 1] = 0;
        
        if(strcmp(line, "SEQUENTIAL") == 0 || strcmp(line, "sequential") == 0)
        {
            out_group->protocol = PROTOCOL_SEQUENTIAL;
        }
        else if(strcmp(line, "QUORUM") == 0 || strcmp(line, "quorum") == 0)
        {
            out_group->protocol = PROTOCOL_QUORUM;
        }
        else if(strcmp(line, "READ-YOUR-WRITE") == 0 || strcmp(line, "READ-YOUR-WRITE") == 0)
        {
            out_group->protocol = PROTOCOL_READ_YOUR_WRITE;
        }
        else
        {
            fprintf(stderr, "ERROR: Invalid protocol specifier \"%s\" found in config file \"%s\"\n", line, file_path);
            error = -1;
            goto exit;
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Config file \"%s\" missing protocol specifier in 1st line\n", file_path);
        error = -1;
        goto exit;
    }

    // clear server count
    out_group->server_count = 0;

    // iterate over remaining lines which represent servers. The first server is the coordinator
    while(fgets(line, sizeof(line), file) != NULL)
    {
        // trim new line char
        line[strlen(line) - 1] = 0;

        colon_found = 0;

        // find colon
        for(int i = 0; i < strlen(line); i++)
        {
            if(line[i] == ':')
            {
                colon_found = 1;
                colon_pos = i;
            }
        }

        if(!colon_found)
        {
            fprintf(stderr, "ERROR: Invalid file configuration. Missing colon in line \"%s\"\n", line);
            error = -1;
            goto exit; 
        }

        // zero out port and address strings 
        memset(str_address, 0, sizeof(str_address));
        memset(str_port, 0, sizeof(str_port));

        // copy over address and port
        memcpy(str_address, line, colon_pos);
        memcpy(str_port, line + colon_pos + 1, strlen(line) - colon_pos);

        // validate address
        if(!tcp_IpAddrIsValid(str_address))
        {
            fprintf(stderr, "ERROR: Invalid file configuration. Bad server address \"%s\" found in \"%s\"\n", str_address, line);
            error = -1;
            goto exit; 
        }

        // validate port
        if(!tcp_PortIsValid(atoi(str_port)))
        {
            fprintf(stderr, "ERROR: Invalid file configuration. Bad server port \"%s\" found in \"%s\"\n", str_port, line);
            error = -1;
            goto exit; 
        }

        // write server and port to output
        if(out_group->server_count == 0)
        {
            strcpy(out_group->primary.address, str_address);
            out_group->primary.port = atoi(str_port);
        }
        else
        {
            strcpy(out_group->others[out_group->server_count - 1].address, str_address);
            out_group->others[out_group->server_count - 1].port = atoi(str_port);
        }

        // increment server count
        out_group->server_count++;
    }

    // close file
    exit:
    fclose(file);

    // return error status
    return error;

}