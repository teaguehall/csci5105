#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "ping.h"

static pthread_t thread_connection;

static void* pingThread(void *vargp)
{
    // attempt to ping server once a second
    while(1)
    {
        // TODO
        

        sleep(1);
    }

    return NULL;
}


int ping_Init(void)
{
    // create thread
    if(pthread_create(&thread_connection, NULL, pingThread, (void*)(NULL)) != 0)
    {
        fprintf(stderr, "ERROR: Failed to spawn connection handler thread. %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0; // TODO

}