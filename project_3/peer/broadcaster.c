#include <stdio.h>
#include <string.h>

#include "broadcaster.h"
#include "send.h"
#include "../shared/server_info.h"
#include "../shared/peer_info.h"

static ServerInfo _server_info;
static PeerInfo _peer_info;
static char _shared_dir[512];

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static pthread_t thread_ping;
static pthread_t thread_file_monitor;

void broadcast(void)
{
    
}

static void* threadPing(void *vargp)
{    
    int recognized;
    
    // attempt to ping server once a second
    while(1)
    {
        // send ping request
        if(send_PingRequest(_server_info, &_peer_info, &recognized) == 0)
        {
            // if we're not recognized by server, we need to broadcast our files
            // TODO broadcast()
        }

        sleep(1);
    }

    return NULL;
}

static void* threadFileMonitor(void *vargp)
{    
    // check for change in files once per second
    while(1)
    {
        printf("Hello from file monitor thread\n");
        
        
        sleep(1);
    }

    return NULL;
}

int broadcaster_Init(ServerInfo* server_info, PeerInfo* peer_info, const char* shared_dir)
{
    // save information related to file publishing
    _server_info = *server_info;
    _peer_info = *peer_info;
    strcpy(_shared_dir, shared_dir);

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
