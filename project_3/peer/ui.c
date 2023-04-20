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

#define RETRY_ATTEMPTS_PER_PEER 5

extern ServerInfo g_server_info;
extern PeerInfo g_our_info;
extern char g_shared_folder[512];

static void* threadUi(void *vargp)
{        
    FileInfo files[MAX_PEERS_IN_NETWORK * MAX_FILES_PER_PEER];
    int num_of_files;
    PeerInfo peers[MAX_PEERS_IN_NETWORK];
    int num_of_peers;
    int i, j;
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
        printf("Enter command: OURS, DISCOVER, FIND, DOWNLOAD, or HELP\n");
                
        if(fgets(command, sizeof(command), stdin) == NULL)
        {
            fprintf(stderr, "ERROR: fgets call failed with error \"%s\"\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        command[strcspn(command, "\n")] = 0; // remove new line character
        render_ClearTerminal(); // always clear terminal to clean up before next output message

        ////////////////////////////////////////////////// OURS HANDLER ///////////////////////////////////////////////////////
        if(strncmp(command, "OURS", sizeof("OURS")) == 0 || strncmp(command, "ours", sizeof("ours")) == 0)
        {
            // get list of files from our shared folder
            broadcaster_GetOurFiles(1, &num_of_files, files);

            // display results
            render_OurFiles(num_of_files, files);
        }

        //////////////////////////////////////////////// DISCOVER HANDLER /////////////////////////////////////////////////////
        else if(strncmp(command, "DISCOVER", sizeof("DISCOVER")) == 0 || strncmp(command, "discover", sizeof("discover")) == 0)
        {
            // send request to server
            if(send_DiscoverRequest(&g_server_info, &num_of_files, files))
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
            if(send_FindRequest(&g_server_info, file_name, &num_of_peers, peers))
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
            if(send_FindRequest(&g_server_info, file_name, &num_of_peers, peers))
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

            // attempt to download file from peer(s)
            downloaded = 0;
            for(i = 0; i < num_of_peers; i++)
            {
                // multiple attempts can be made per peer
                for(j = 1; j < (RETRY_ATTEMPTS_PER_PEER + 1); j++)
                {
                    printf("Attempt #%d to download file \"%s\" from peer %s:%d...\n", j, file_name, peers[i].listening_addr, peers[i].listening_port);
                    if(send_DownloadRequest(peers + i, file_name, &downloaded_file_info, downloaded_bytes) == 0)
                    {
                        // validate file check sum
                        if(downloaded_file_info.check_sum == checksum_bytes(downloaded_file_info.size, downloaded_bytes))
                        {
                            // we succeeded with download, now write it to disk
                            file_writer(g_shared_folder, &downloaded_file_info, downloaded_bytes);
                            downloaded = 1;
                            goto done;  
                        }
                        else
                        {
                            
                            printf("Download failed checksum validation\n");
                        }
                    }
                }
            }

            // print status message of outcome
            done: 
            if(downloaded)
            {
                printf("Successfully downloaded \"%s\" from peer %s:%d\n\n", file_name, peers[i].listening_addr, peers[i].listening_port);
            }
            else
            {
                fprintf(stderr, "ERROR: Failed to download file \"%s\"\n\n", file_name);
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

int ui_Init(void)
{
    pthread_t thread_ui;

    // spawn ui thread
    if(pthread_create(&thread_ui, NULL, threadUi, NULL) != 0)
    {
        fprintf(stderr, "ERROR: Broadcaster failed to spawn file monitoring thread. %s\n", strerror(errno));
        return -1;
    }

    // success
    return 0;
}
