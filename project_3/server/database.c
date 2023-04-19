// quick and dirty data structure for tracking peer files. could probably write something more efficient...

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "database.h"
#include "../shared/assumptions.h"

#define TIMEOUT_SEC 5

static int peer_count = 0;

typedef struct DbEntry
{
    int active;
    PeerInfo peer;
    FileInfo files[MAX_FILES_PER_PEER];
    int file_count;
    time_t timestamp;

} DbEntry;

DbEntry db[MAX_PEERS_IN_NETWORK];

pthread_mutex_t db_lock = PTHREAD_MUTEX_INITIALIZER;

static void* threadTimeoutMonitor(void *vargp)
{    
    time_t curr_time;
    
    // monitor for peer timeouts
    while(1)
    {
        curr_time = time(NULL);
        
        // grab mutex lock
        if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}

        // check for timed out peers
        for(int i = 0; i < MAX_PEERS_IN_NETWORK; i++)
        {
            if(db[i].active && (curr_time - db[i].timestamp) > TIMEOUT_SEC)
            {
                db[i].active = 0;
                peer_count--;
                printf("INFO: Peer \"%s:%d\" left the network. Peer Count = %d\n", db[i].peer.listening_addr, db[i].peer.listening_port, peer_count);
            }
        }
        
        // release mutex lock
        if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}

        sleep(1); 
    }
    
    return NULL;
}

// initializes database
int db_Init(void)
{   
    pthread_t thread_timeout_monitor;
    
    // grab mutex lock
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}
    
    // intialize all db entries as inactive
    for(int i = 0; i < MAX_PEERS_IN_NETWORK; i++)
    {
        db[i].active = 0;
    }

    // release mutex lock
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}

    // start timeout monitor thread
    if(pthread_create(&thread_timeout_monitor, NULL, threadTimeoutMonitor, NULL) != 0)
    {
        fprintf(stderr, "ERROR: Db failed to spawn timeout monitor thread. %s\n", strerror(errno));
        return -1;
    }

    // success
    return 0;
}

// add (or update existing) peer within database
int db_AddPeer(const PeerInfo* peer, int num_of_files, const FileInfo files[])
{
    int error = 0; // assume success    
    
    // grab mutex lock
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}
    
    // update peer entry if it already exists in the database
    for(int i = 0; i < MAX_PEERS_IN_NETWORK; i++)
    {
        // only check active peers
        if(db[i].active)
        {
            // check address and port
            if(strcmp(peer->listening_addr, db[i].peer.listening_addr) == 0 && peer->listening_port == db[i].peer.listening_port)
            {
                // print info message that number of shared files has changed
                if(db[i].file_count != num_of_files)
                {
                    printf("INFO: Peer \"%s:%d\" now shares %d files\n", db[i].peer.listening_addr, db[i].peer.listening_port, num_of_files);
                }
                
                // update database entry
                db[i].file_count = num_of_files;
                memcpy(db[i].files, files, num_of_files * sizeof(FileInfo));
                db[i].timestamp = time(NULL);

                // leave
                goto unlock;
            }
        }
    }

    // peer was not previously found in database, let's add it now
    for(int i = 0; i < MAX_PEERS_IN_NETWORK; i++)
    {
        // find open slot
        if(!db[i].active)
        {
            db[i].active = 1;
            db[i].peer = *peer;
            db[i].file_count = num_of_files;
            memcpy(db[i].files, files, num_of_files * sizeof(FileInfo));
            db[i].timestamp = time(NULL);

            peer_count++;
            printf("INFO: Peer \"%s:%d\" joined the network with %d files. Peer Count = %d\n", db[i].peer.listening_addr, db[i].peer.listening_port, num_of_files, peer_count);

            // leave
            goto unlock;
        }
    }

    // if we reached this point, that means we couldn't find an open slot. throw an error
    error = -1;

    // release mutex lock
    unlock:
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}

    return error;
}

// updates the peer timestamp in the database to prevent timeout
int db_KeepAlive(const PeerInfo* peer)
{
    int error = 0; // assume success    
    
    // grab mutex lock
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}
    
    // update peer entry if it already exists in the database
    for(int i = 0; i < MAX_PEERS_IN_NETWORK; i++)
    {
        // only check active peers
        if(db[i].active)
        {
            // check address and port
            if(strcmp(peer->listening_addr, db[i].peer.listening_addr) == 0 && peer->listening_port == db[i].peer.listening_port)
            {
                // update database timestamp to current time
                db[i].timestamp = time(NULL);

                // leave
                goto unlock;
            }
        }
    }

    // if we reached this point, that means we don't recognize the peer
    error = -1;

    // release mutex lock
    unlock:
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}

    return error;    
}

// returns all peers who have the inquiried file
int db_FindFile(const char* file_name, int* out_num_of_peers, PeerInfo out_peers[])
{
    *out_num_of_peers = 0; 
    
    // grab mutex lock
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}
    
    // iterate over all peers
    for(int i = 0; i < MAX_PEERS_IN_NETWORK; i++)
    {
        // only check active peers
        if(db[i].active)
        {
            // iterate over each file on each peer
            for(int j = 0; j < db[i].file_count; j++)
            {
                // copy db peer to output if filename match found
                if(strcmp(db[i].files[j].name, file_name) == 0)
                {
                    out_peers[(*out_num_of_peers)++] = db[i].peer;
                    break; // move onto next peer
                } 
            }
        }
    }

    // release mutex lock
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}

    // success
    return 0;        
}

// returns all unique files from database
int db_DiscoverFiles(int* out_num_of_files, FileInfo out_files[])
{    
    int duplicate;
    
    // clear the number of outputted files
    *out_num_of_files = 0; 
    
    // grab mutex lock
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}
    
    // iterate over each active peer
    for(int i = 0; i < MAX_PEERS_IN_NETWORK; i++)
    {
        if(db[i].active)
        {
            // iterate over each file on each peer
            for(int j = 0; j < db[i].file_count; j++)
            {
                duplicate = 0;
                
                // verify we have not already outputted this file name (don't want to output duplicates)
                for(int k = 0; k < *out_num_of_files; k++)
                {
                    if(strcmp(db[i].files[j].name, out_files[*out_num_of_files].name) == 0)
                    {
                        duplicate = 1;
                        break;
                    }
                }

                // add file to output if it's not a duplicate
                if(!duplicate)
                {
                    out_files[(*out_num_of_files)++] = db[i].files[j];
                }
            }
        }
    }

    // release mutex lock
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock database mutex. %s. Killing server...", strerror(errno)); exit(EXIT_FAILURE);}

    // success
    return 0;     
}
