#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "server_msg_outbox.h"
#include "server_msg.h"

#define MSG_OUTBOX_SIZE 4 // [number of messages]
char outbox[MSG_OUTBOX_SIZE][MAX_SERVER_MSG_SIZE];

static int outbox_count = 0;
static int outbox_next_write = 0;
static int outbox_next_read = 0;

pthread_mutex_t outbox_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t outbox_notempty;
pthread_cond_t outbox_notfull;

// inserts message into outbox. returns 0 on success, -1 on error
int serverMsgOutbox_Insert(char* msg_to_insert)
{
    int error = 0;
    size_t msg_size;

    // lock mutex
    if(pthread_mutex_lock(&outbox_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock outbox mutex. %s", strerror(errno)); return -1;}

    // if outbox is full, wait for available opening
    while(outbox_count == MSG_OUTBOX_SIZE)
    {
        fprintf(stderr, "WARN: Server outbox is full %d/%d. Waiting until opening becomes available\n", MSG_OUTBOX_SIZE, MSG_OUTBOX_SIZE);
        pthread_cond_wait(&outbox_notfull, &outbox_lock);
    }

    // copy message to next available outbox position
    msg_size = msg_GetActualSize(msg_to_insert);
    memcpy(outbox[outbox_next_write], msg_to_insert, msg_size);

    // update outbox count and signal that outbox is not empty
    outbox_count++;
    pthread_cond_signal(&outbox_notempty);

    // update next add position
    outbox_next_write = (outbox_next_write + 1) % MSG_OUTBOX_SIZE;
    
    // unlock mutex
    if(pthread_mutex_unlock(&outbox_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock server outbox mutex. %s", strerror(errno)); return -1;}

    // return either 0 or -1 
    return error;
}

// reads next available message from outbox (while also removing it from the outbox)
int serverMsgOutbox_Read(char* out_msg)
{
    int error = 0;
    size_t msg_size;

    // lock mutex
    if(pthread_mutex_lock(&outbox_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock outbox mutex. %s", strerror(errno)); return -1;}

    // if outbox is empty, sleep until new message arrives
    while(outbox_count == 0)
        pthread_cond_wait(&outbox_notempty, &outbox_lock);

    // copy message to next available outbox position
    msg_size = msg_GetActualSize(outbox[outbox_next_read]);
    memcpy(out_msg, outbox[outbox_next_read], msg_size);

    // decrement outbox count and signal that outbox is not full
    outbox_count--;
    pthread_cond_signal(&outbox_notfull);

    // update next read position
    outbox_next_read = (outbox_next_read + 1) % MSG_OUTBOX_SIZE;
    
    // unlock mutex
    if(pthread_mutex_unlock(&outbox_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock server outbox mutex. %s", strerror(errno)); return -1;}

    // return either 0 or -1 
    return error;
}

