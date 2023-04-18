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

static ServerInfo _server_info;
static PeerInfo _peer_info;
static char _shared_dir[512];

static void get_file_info(int do_checksum, int* out_file_count, FileInfo out_files[])
{
    DIR* pdir;
    struct dirent* pentry;

    *out_file_count = 0;
    
    // open directory
    pdir = opendir(_shared_dir);
    if(pdir == NULL)
    {
        fprintf(stderr, "Error opening shared folder \"%s\": %s. Exitting...\n", _shared_dir, strerror(errno));
        goto close;
    }

    // iterate over files
    while((pentry = readdir(pdir)) != NULL && (*out_file_count < MAX_FILES_PER_NODE)) 
    {
        // ignore the . and .. entries
        if(strcmp(pentry->d_name, ".") == 0 || strcmp(pentry->d_name, "..") == 0)
        {
            continue;
        }

        // copy file name to output
        strcpy(out_files[*out_file_count].name, pentry->d_name);
        printf("file name = %s\n", pentry->d_name);

        // calculate file size and checksum if asked to do so
        if(do_checksum)
        {
            if(checksum(out_files + (*out_file_count)))
            {
                fprintf(stderr, "Failed to compute checksum on \"%s\"\n", out_files[*out_file_count].name);
                goto close;
            }
        }

        // increment file count
        (*out_file_count)++;
    }

    // close directory
    close:
    closedir(pdir);        
}

static void broadcast(void)
{
    int file_count;
    FileInfo files[MAX_FILES_PER_NODE];
    
    // get file info
    get_file_info(1, &file_count, files);

    // broadcast files to server
    if(send_UpdateListRequest(&_server_info, &_peer_info, file_count, files))
    {
        fprintf(stderr, "WARN: Failed to broadcast files to server.");
    }
}

static void* threadPing(void *vargp)
{    
    int recognized;
    
    // attempt to ping server once a second
    while(1)
    {
        // send ping request
        if(send_PingRequest(&_server_info, &_peer_info, &recognized))
        {
            fprintf(stderr, "WARN: Failed to ping server.");
        }
        else
        {
            // if server doesn't recognize us, broadcast our info
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
    FileInfo prev_files[MAX_FILES_PER_NODE];
    FileInfo curr_files[MAX_FILES_PER_NODE];
    
    // check for change in files once per second
    while(1)
    {
        // get updated file information
        get_file_info(0, &curr_count, curr_files);

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

int broadcaster_Init(ServerInfo* server_info, PeerInfo* peer_info, const char* shared_dir)
{
    pthread_t thread_ping;
    pthread_t thread_file_monitor;
    
    // save information related to file publishing
    _server_info = *server_info;
    _peer_info = *peer_info;
    strcpy(_shared_dir, shared_dir);

    // TODO delete
    broadcast();
    return 0;

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
