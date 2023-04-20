#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include "ui.h"
#include "render.h"
#include "broadcaster.h"
#include "send.h"
#include "file.h"
#include "checksum.h"
#include "peer_selection.h"
#include "../shared/server_info.h"
#include "../shared/peer_info.h"
#include "../shared/assumptions.h"

static ServerInfo _server_info;
static PeerInfo _peer_info;
static char _shared_dir[512];

static void* threadUi(void *vargp)
{        
    FileInfo files[MAX_PEERS_IN_NETWORK * MAX_FILES_PER_PEER];
    int num_of_files;
    PeerInfo peers[MAX_PEERS_IN_NETWORK];
    int num_of_peers;
    int i;
    char* file_name;
    FileInfo downloaded_file_info;
    char downloaded_bytes[MAX_FILE_SIZE_BYTES];
    int downloaded; 
    
    char command[1024];
    
    // clear terminal
    render_ClearTerminal();
    render_HelpMenu();
    
    // attempt to ping server once a second
    while(1)
    {
        ////////////////////////////////////////////////// GET USER INPUT /////////////////////////////////////////////////////
        printf("Enter command: DISCOVER, FIND, DOWNLOAD, or HELP\n");
                
        if(fgets(command, sizeof(command), stdin) == NULL)
        {
            fprintf(stderr, "ERROR: fgets call failed with error \"%s\"\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        command[strcspn(command, "\n")] = 0; // remove new line character
        render_ClearTerminal(); // always clear terminal to clean up before next output message

        //////////////////////////////////////////////// DISCOVER HANDLER /////////////////////////////////////////////////////
        if(strncmp(command, "DISCOVER", sizeof("DISCOVER")) == 0 || strncmp(command, "discover", sizeof("discover")) == 0)
        {
            // send request to server
            if(send_DiscoverRequest(&_server_info, &num_of_files, files))
            {
                printf("ERROR: Failed to discover files from server\n");
                continue;
            }

            // display results
            render_DiscoveredFiles(num_of_files, files);
        }

        /////////////////////////////////////////////////// FIND HANDLER //////////////////////////////////////////////////////
        else if(strncmp(command, "FIND", strlen("FIND")) == 0 || strncmp(command, "find", strlen("find")) == 0)
        {
            file_name = command + 5; // command = "FIND;<file name>", thus we add an offset of 5 to the command input
            
            // send request to server
            if(send_FindRequest(&_server_info, file_name, &num_of_peers, peers))
            {
                printf("ERROR: Failed to find files from server\n");
                continue;
            }

            // display results
            render_PeersWithFile(file_name, num_of_peers, peers);
        }

        ///////////////////////////////////////////////// DOWNLOAD HANDLER ////////////////////////////////////////////////////
        else if(strncmp(command, "DOWNLOAD", strlen("DOWNLOAD")) == 0 || strncmp(command, "download", strlen("download")) == 0)
        {
            file_name = command + 9; // command = "DOWNLOAD;<file name>", thus we add an offset of 9 to the command input
            
            // send request to server find eligble peers
            if(send_FindRequest(&_server_info, file_name, &num_of_peers, peers))
            {
                printf("ERROR: Failed to find files from server\n");
                continue;
            }

            // throw error if no file was found
            if(num_of_peers == 0)
            {
                printf("ERROR: Requested file \"%s\" does not exist on the network.\n", file_name);
                continue;
            }

            // run select algorithm on the set of peers
            // note: this simply sorts the peers based on "best to worst"
            peerSelection_Sort(num_of_peers, peers);

            // attempt to download file from peer
            downloaded = 0;
            for(i = 0; i < num_of_peers; i++)
            {
                if(send_DownloadRequest(peers + i, file_name, &downloaded_file_info, downloaded_bytes) == 0)
                {
                    // we succeeded with download, now write it to disk
                    file_writer(&downloaded_file_info, downloaded_bytes);
                    downloaded = 1;
                    break; 
                }
            }

            // print error message if no downloads were successful
            if(!downloaded)
            {
                fprintf(stderr, "ERROR: Failed to download file \"%s\"\n", file_name);
            }
        }

        /////////////////////////////////////////////////// HELP HANDLER //////////////////////////////////////////////////////
        else if(strncmp(command, "HELP", strlen("HELP")) == 0 || strncmp(command, "help", strlen("help")) == 0)
        {            
            // display success message
            render_HelpMenu();
        }

        ////////////////////////////////////////////// UNKNOWN REQUEST HANDLER /////////////////////////////////////////////////
        else
        {
            fprintf(stderr, "WARN: Unexpected command \"%s\" received. Try again...\n", command);
        }
    }

    return NULL;
}

int ui_Init(ServerInfo* server_info, PeerInfo* peer_info, const char* shared_dir)
{
    pthread_t thread_ui;
    
    // save information related to file publishing
    _server_info = *server_info;
    _peer_info = *peer_info;
    strcpy(_shared_dir, shared_dir);

    // spawn ui thread
    if(pthread_create(&thread_ui, NULL, threadUi, NULL) != 0)
    {
        fprintf(stderr, "ERROR: Broadcaster failed to spawn file monitoring thread. %s\n", strerror(errno));
        return -1;
    }

    // success
    return 0;
}
