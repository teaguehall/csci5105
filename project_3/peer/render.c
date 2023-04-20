#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../shared/server_info.h"

extern ServerInfo g_server_info;
extern PeerInfo g_our_info;
extern char g_shared_folder[512];

#define DIVIDER_LINE "---------------------------------------------------------------------------\n"

void render_ClearTerminal(void)
{
    if(system("clear") == -1)
    {
        fprintf(stderr, "ERROR: Failed to clear terminal during article rendering: %s\n", strerror(errno));
    }
}

void render_HelpMenu(void)
{
    // display message formatting instructions
    printf(DIVIDER_LINE);
    printf("Supported Messages + Formatting Instructions\n");
    printf(DIVIDER_LINE);
    printf("%-24s%s\n", "OURS", "// displays files in our shared folder");
    printf("%-24s%s\n", "DISCOVER", "// displays all files on network");
    printf("%-24s%s\n", "FIND <file name>", "// lists peers that host specified file");
    printf("%-24s%s\n", "DOWNLOAD <file name>", "// downloads file (from best peer)");
    printf(DIVIDER_LINE);
    printf("\n");
}

void render_DiscoveredFiles(int num_of_files, const FileInfo files[])
{
    // print header
    printf(DIVIDER_LINE);
    printf("Discovered Files: %d\n", num_of_files);
    printf(DIVIDER_LINE);
    printf("%-24s%-24s%-24s\n\n", "name", "size (bytes)", "checksum");

    // print files (or none)
    if(num_of_files)
    {
        for(int i = 0; i < num_of_files; i++)
        {
            printf("%-24s%-24u%-24u\n", files[i].name, files[i].size, files[i].check_sum);
        }
    }
    else
    {
        printf("No files found!\n");
    }

    // print fooder
    printf(DIVIDER_LINE);
    printf("\n");
}

void render_OurFiles(int num_of_files, const FileInfo files[])
{
    // print header
    printf(DIVIDER_LINE);
    printf("Our Files: %d\n", num_of_files);
    printf(DIVIDER_LINE);
    printf("%-24s%-24s%-24s\n\n", "name", "size (bytes)", "checksum");

    // print files (or none)
    if(num_of_files)
    {
        for(int i = 0; i < num_of_files; i++)
        {
            printf("%-24s%-24u%-24u\n", files[i].name, files[i].size, files[i].check_sum);
        }
    }
    else
    {
        printf("We have no files!\n");
    }

    // print fooder
    printf(DIVIDER_LINE);
    printf("\n");
}

void render_PeersWithFile(const char* file_name, int num_of_peers, const PeerInfo peers[])
{
    // print header
    printf(DIVIDER_LINE);
    printf("%d peer(s) found with file \"%s\"\n", num_of_peers, file_name);
    printf(DIVIDER_LINE);
    

    // print peers
    if(num_of_peers)
    {
        printf("%-16s%-16s%-16s%-16s\n\n", "address", "port", "lat.", "long.");

        for(int i = 0; i < num_of_peers; i++)
        {
            // make a little symbol indicating which peer is "us"
            if(strcmp(peers[i].listening_addr, g_our_info.listening_addr) == 0 && peers[i].listening_port == g_our_info.listening_port) 
            {
                printf("%-16s%-16d%-16f%-16f<-- \"us\"\n", peers[i].listening_addr, peers[i].listening_port, peers[i].latitude, peers[i].longitude);
            }
            else
            {
                printf("%-16s%-16d%-16f%-16f\n", peers[i].listening_addr, peers[i].listening_port, peers[i].latitude, peers[i].longitude);
            }
            
        }

        // print footer
        printf(DIVIDER_LINE);
        printf("\n");
    }
    else
    {
        printf("Typo in file name? Try again.\n\n");
    }
}
