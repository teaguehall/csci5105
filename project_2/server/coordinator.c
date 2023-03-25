#include <stdio.h>

#include "server_msg_outbox.h"
#include "../shared/msg.h"

void* funcCoordinator(void *vargp)
{
    char msg[4096];
    printf("Hello from coordinator!\n");

    // build message
    msg_Build_PostRequest(msg, "Tim", "Help Wanted!", "Looking for an electrician");

    return NULL;
}