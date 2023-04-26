#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "peer_selection.h"
#include "send.h"
#include "../shared/assumptions.h"

void peerSelection_Sort(int bytes_to_download, int num_of_peers, PeerInfo peers[])
{
    struct timespec tbefore, tafter;
    double latency_sec;

    int bytes_per_sec;
    
    //PeerInfo sorted_peers[MAX_PEERS_IN_NETWORK];
    double downloadtime[MAX_PEERS_IN_NETWORK];
    
    // get load from each 
    for(int i = 0; i < num_of_peers; i++)
    {
        // grab start time (to calculate latency)
        clock_gettime(CLOCK_REALTIME, &tbefore);

        // send get load request to each peer
        if(send_GetLoadRequest(peers + i, &bytes_per_sec))
        {
            downloadtime[i] = -1;   // mark as bad download time if we are unable to reach peer
            continue;
        }
        
        printf("bytes per sec = %d\n", bytes_per_sec); // TODO delete

        // grab end time (to calculate latency)
        clock_gettime(CLOCK_REALTIME, &tafter);

        // calculate latency
        latency_sec = (tafter.tv_sec - tbefore.tv_sec) + (tafter.tv_nsec - tbefore.tv_nsec) / 1000000000.0;

        // calculate download time
        downloadtime[i] = latency_sec + ((double)bytes_to_download) / bytes_per_sec ;

        // debug print
        printf("latency = %f secs, download = %f secs\n", latency_sec, downloadtime[i]);

    }
}