#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "load_tracker.h"

pthread_mutex_t loads_lock = PTHREAD_MUTEX_INITIALIZER;
static int loads = 0;

void loadTracker_Add(void)
{
    // grab mutex lock
    if(pthread_mutex_lock(&loads_lock) != 0) {fprintf(stderr, "FATAL: Failed to grab load tracker lock. %s. Killing peer...", strerror(errno)); exit(EXIT_FAILURE);}  

    loads++;

    // release mutex lock
    if(pthread_mutex_unlock(&loads_lock) != 0) {fprintf(stderr, "FATAL: Failed to release load tracker lock. %s. Killing peer...", strerror(errno)); exit(EXIT_FAILURE);}
}

void loadTracker_Sub(void)
{
    // grab mutex lock
    if(pthread_mutex_lock(&loads_lock) != 0) {fprintf(stderr, "FATAL: Failed to grab load tracker lock. %s. Killing peer...", strerror(errno)); exit(EXIT_FAILURE);}  

    loads--;

    // release mutex lock
    if(pthread_mutex_unlock(&loads_lock) != 0) {fprintf(stderr, "FATAL: Failed to release load tracker lock. %s. Killing peer...", strerror(errno)); exit(EXIT_FAILURE);}
}

int loadTracker_Get(void)
{
    int loads_local;
    
    // grab mutex lock
    if(pthread_mutex_lock(&loads_lock) != 0) {fprintf(stderr, "FATAL: Failed to grab load tracker lock. %s. Killing peer...", strerror(errno)); exit(EXIT_FAILURE);}  

    loads_local = loads;

    // release mutex lock
    if(pthread_mutex_unlock(&loads_lock) != 0) {fprintf(stderr, "FATAL: Failed to release load tracker lock. %s. Killing peer...", strerror(errno)); exit(EXIT_FAILURE);}

    return loads_local;
}