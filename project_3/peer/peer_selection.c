#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <float.h>
#include <string.h>

#include "peer_selection.h"
#include "send.h"
#include "../shared/assumptions.h"

#define DIVIDER_LINE "---------------------------------------------------------------------------\n"

typedef struct PeerDownload
{
    double download_time;
    PeerInfo peer_info;

} PeerDownload;

int compare(const void* a, const void* b) 
{
   return (((PeerDownload*)a)->download_time > ((PeerDownload*)b)->download_time);
}

void peerSelection_Sort(int bytes_to_download, int num_of_peers, PeerInfo peers[])
{
    struct timespec tbefore, tafter;
    double latency_sec;
    int bytes_per_sec;
    int i;

    PeerDownload download_estimates[MAX_PEERS_IN_NETWORK];

    // copy peer info from input into download array
    for(i = 0; i < num_of_peers; i++)
    {
        memcpy(&(download_estimates[i].peer_info), peers + i, sizeof(PeerInfo));
        
        // initialize download estimate as largest possible time
        download_estimates[i].download_time = DBL_MAX;
    }

    // print
    printf("Estimating download times from eligible peers...\n");
    printf(DIVIDER_LINE);
    
    // get load from each peer 
    for(int i = 0; i < num_of_peers; i++)
    {
        // grab start time (to calculate latency)
        clock_gettime(CLOCK_REALTIME, &tbefore);

        // send get load request to each peer
        if(send_GetLoadThoughputRequest(peers + i, &bytes_per_sec))
        {
            printf("Peer (%s:%d): Unable to query load...\n", peers[i].listening_addr, peers[i].listening_port);
            continue;
        }

        // grab end time (to calculate latency)
        clock_gettime(CLOCK_REALTIME, &tafter);

        // calculate latency
        latency_sec = (tafter.tv_sec - tbefore.tv_sec) + (tafter.tv_nsec - tbefore.tv_nsec) / 1000000000.0;

        // calculate download time
        download_estimates[i].download_time = latency_sec + ((double)bytes_to_download) / bytes_per_sec;

        printf("Peer (%s:%d): Latency = %.2f sec, ThroughPut = %.2f Mbyte/sec, Estimate = %.2f sec\n", peers[i].listening_addr, peers[i].listening_port, latency_sec, (double)bytes_per_sec / 10000000, download_estimates[i].download_time);
    }

    // sort the peers by download times
    qsort(download_estimates, num_of_peers, sizeof(PeerDownload), compare);

    printf("\n");
    printf("Peers sorted by download times (best to worst)\n");
    printf(DIVIDER_LINE);

    // copy the downloads back to the output
    for(i = 0; i < num_of_peers; i++)
    {
        memcpy(peers + i, &(download_estimates[i].peer_info), sizeof(PeerInfo));
        printf("Peer (%s:%d): Estimate = %f sec\n", peers[i].listening_addr, peers[i].listening_port, download_estimates[i].download_time);
    }

    printf("\n");
}