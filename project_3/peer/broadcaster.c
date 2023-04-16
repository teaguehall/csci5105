#include <stdio.h>
#include <string.h>

#include "broadcaster.h"


static ServerInfo _server_info;
static PeerInfo _server_info;
static char _shared_dir[512];

int broadcaster_Init(ServerInfo* server_info, PeerInfo* peer_info, const char* shared_dir);
{
    // save inputted data for later
    _server_info = *server_info;
    _peer_info = *peer_info;
    strcpy(_shared_dir, shared_dir)
    
    
    // start folder monitoring thread
    // TODO


    // 
}

int broadcaster_Force(void)
{
    signals broadcast thread to wake up...
}


int broadcaster_Force(void);