#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "broadcaster.h"
#include "send.h"
#include "checksum.h"
#include "../shared/server_info.h"
#include "../shared/peer_info.h"
#include "../shared/assumptions.h"

extern ServerInfo g_server_info;
extern PeerInfo g_our_info;
extern char g_shared_folder[512];

int broadcaster_GetOurFiles(int do_checksum, int* out_file_count, FileInfo out_files[])
{
    int error = 0; // assume success
    
    DIR* pdir;
    struct dirent* pentry;

    *out_file_count = 0;
    
    // open directory
    pdir = opendir(g_shared_folder);
    if(pdir == NULL)
    {
        fprintf(stderr, "Error opening shared folder \"%s\": %s. Exitting...\n", g_shared_folder, strerror(errno));
        error = 1;
        goto close;
    }

    // iterate over files
    while((pentry = readdir(pdir)) != NULL && (*out_file_count < MAX_FILES_PER_PEER)) 
    {
        // ignore the . and .. entries
        if(strcmp(pentry->d_name, ".") == 0 || strcmp(pentry->d_name, "..") == 0)
        {
            continue;
        }

        // copy file name to output
        strcpy(out_files[*out_file_count].name, pentry->d_name);
        
        // calculate file size and checksum if asked to do so
        if(do_checksum)
        {
            if(checksum_file(g_shared_folder, out_files + (*out_file_count)))
            {
                fprintf(stderr, "Failed to compute checksum on \"%s\"\n", out_files[*out_file_count].name);
                error = 1;
                goto close;
            }
        }

        // TODO delete debug
        //printf("Name: %s, Size: %u, CRC: %u\n", out_files[*out_file_count].name, out_files[*out_file_count].size, out_files[*out_file_count].check_sum);

        // increment file count
        (*out_file_count)++;
    }

    // close directory
    close:
    closedir(pdir);       

    // return error status
    return error;
}

static void broadcast(void)
{    
    int file_count;
    FileInfo files[MAX_FILES_PER_PEER];
    
    // get file info
    broadcaster_GetOurFiles(1, &file_count, files);

    // broadcast files to server
    if(send_UpdateListRequest(&g_server_info, &g_our_info, file_count, files))
    {
        fprintf(stderr, "WARN: Failed to broadcast files to server.\n");
    }
}

static void* threadPing(void *vargp)
{    
    int recognized;
    
    // attempt to ping server once a second
    while(1)
    {
        // send ping request
        if(send_PingRequest(&g_server_info, &g_our_info, &recognized))
        {
            fprintf(stderr, "WARN: Failed to ping server.");
        }
        else
        {
            // if server doesn't recognize us (because it crashed and restarted...), broadcast our info
            if(!recognized)
            {
                broadcast();
            }
        }

        sleep(1);
    }

    return NULL;
}

static void* threadFileMonitor(void *vargp)
{
    int prev_count = 0;
    int curr_count = 0;
    FileInfo prev_files[MAX_FILES_PER_PEER];
    FileInfo curr_files[MAX_FILES_PER_PEER];
    
    // check for change in files once per second
    while(1)
    {
        // get updated file information
        broadcaster_GetOurFiles(0, &curr_count, curr_files);

        // broadcast files if they have changed
        if(curr_count != prev_count)
        {
            broadcast();
        }
        else
        {
            // iterate and compare file names to detect any changes
            for(int i = 0; i < curr_count; i++)
            {
                if(strcmp(curr_files[i].name, prev_files[i].name) != 0)
                {
                    broadcast();
                    break;
                }
            }
        }

        // save curr info as prev for next time around
        prev_count = curr_count;
        memcpy(prev_files, curr_files, curr_count * sizeof(FileInfo));

        // sleep for a bit
        sleep(1);
    }

    return NULL;
}

int broadcaster_Init()
{
    pthread_t thread_ping;
    pthread_t thread_file_monitor;

    // spawn file monitor thread
    if(pthread_create(&thread_file_monitor, NULL, threadFileMonitor, NULL) != 0)
    {
        fprintf(stderr, "ERROR: Broadcaster failed to spawn file monitoring thread. %s\n", strerror(errno));
        return -1;
    }

    // spawn ping thread
    if(pthread_create(&thread_ping, NULL, threadPing, NULL) != 0)
    {
        fprintf(stderr, "ERROR: Broadcaster failed to spawn ping thread. %s\n", strerror(errno));
        return -1;
    }

    // success
    return 0;
}
