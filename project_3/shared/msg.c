#include "msg.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

pthread_mutex_t next_id_lock = PTHREAD_MUTEX_INITIALIZER;

int32_t msg_GetNextID(void)
{
    static int32_t id = 0;
    int32_t id_copy;
    
    // lock mutex
    if(pthread_mutex_lock(&next_id_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock msg build mutex. %s", strerror(errno)); return -1;}

    id_copy = id++;

    // unlock mutex
    if(pthread_mutex_unlock(&next_id_lock) != 0) {fprintf(stderr, "ERROR: Failed to unlock msg build mutex. %s", strerror(errno)); return -1;}

    return id_copy;
}

int msg_GetID(const char* msg)
{
    uint32_t type = 0;
    int32_t id = 0;
    uint32_t size = 0;
    
    msg_Parse_Header(msg, &type, &id, &size);

    return (size + MSG_HEADER_SIZE);
}

size_t msg_GetActualSize(const char* msg)
{
    uint32_t type = 0;
    int32_t id = 0;
    uint32_t size = 0;
    
    msg_Parse_Header(msg, &type, &id, &size);

    return (size + MSG_HEADER_SIZE);
}

// Builds message header. Always succeeds and returns 0
int msg_Build_Header(char* out_msg, uint32_t type,  uint32_t size)
{
    // write magic number
    (*(uint32_t*)out_msg) = htonl(MSG_MAGIC_NUMBER);

    // write message type
    (*(uint32_t*)(out_msg + 4)) = htonl(type);

    // write message ID
    (*(uint32_t*)(out_msg + 8)) = htonl(msg_GetNextID());

    // write message size (excludes size of header itself)
    (*(uint32_t*)(out_msg + 12)) = htonl(size);

    // success
    return 0;
}

// Parse message header. Returns 0 on success, -1 on error
int msg_Parse_Header(const char* in_msg, uint32_t* out_type, int32_t* out_id, uint32_t* out_size)
{
    uint32_t magic = ntohl(*((uint32_t*)in_msg));
    *out_type = ntohl(*(uint32_t*)(in_msg + 4));
    *out_id = ntohl(*(int32_t*)(in_msg + 8));
    *out_size = ntohl(*(uint32_t*)(in_msg + 12));

    // validate header
    if(magic != MSG_MAGIC_NUMBER)
    {
        fprintf(stderr, "ERROR: Invalid magic number in header. Found %d, expected %d\n", magic, MSG_MAGIC_NUMBER);
        return -1;
    }

    // success
    return 0;
}

// Builds MSG_TYPE_ERROR_RESPONSE message. Returns 0 on success, -1 on error
int msg_Build_ErrorResponse(char* out_msg, char* in_err_string)
{
    uint32_t bytes_written = 0;
    
    // copy error string to message
    strcpy(out_msg + MSG_HEADER_SIZE, in_err_string);
    bytes_written += strlen(in_err_string) + 1;

    // write message header
    msg_Build_Header(out_msg, MSG_TYPE_ERROR_RESPONSE,  bytes_written);

    // success
    return 0;
}

// Parses MSG_TYPE_ERROR_RESPONSE message. Returns 0 on success, -1 on error
int msg_Parse_ErrorResponse(const char* in_msg, char* out_error_msg)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_ERROR_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_ERROR_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_ERROR_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_ERROR_RESPONSE);
        return -1;
    }

    // copy string to output
    strcpy(out_error_msg, in_msg + MSG_HEADER_SIZE);

    // success
    return 0;
}

int msg_Build_PingRequest(char* out_msg, const PeerInfo* peer)
{
    return 0; // TODO define
}

int msg_Parse_PingResponse(char* out_msg, int* out_recognized)
{
    return 0; // TODO
}

int msg_Build_UpdateListRequest(char* out_msg, const PeerInfo* peer_info, int num_of_files, const FileInfo files[])
{
    return 0; // TODO
}
