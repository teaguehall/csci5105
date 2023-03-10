#include "client_handler.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

void* client_Handler(void *vargp)
{
    // cast input args
    char* listening_addr = ((ClientHandlerInfo*)(vargp))->listening_address;
    int listening_port = ((ClientHandlerInfo*)(vargp))->listening_port;

    // print
    printf("Hello from client handler: %s:%d\n", listening_addr, listening_port);

    // success
    return NULL;
}

