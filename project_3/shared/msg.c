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

int msg_Build_DiscoverRequest(char* out_msg)
{
    // write header
    msg_Build_Header(out_msg, MSG_TYPE_DISCOVER_REQUEST, 0);

    // success
    return 0;  
}

int msg_Build_DiscoverResponse(char* out_msg, int num_of_files, const FileInfo files[])
{
    uint32_t bytes_written = 0;

    // write article count
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(num_of_files);
    bytes_written += 4;

    // write each arcticle
    for(int i = 0; i < num_of_files; i++)
    {
        // write file name
        strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, files[i].name);
        bytes_written += strlen(files[i].name) + 1;
        
        // write file size
        *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(files[i].size);
        bytes_written += 4;

        // write check sum
        *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(files[i].check_sum);
        bytes_written += 4;

    }

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_DISCOVER_RESPONSE,  bytes_written);
    
    // success
    return 0;
}

int msg_Parse_DiscoverResponse(const char* in_msg, int* out_num_of_files, FileInfo out_files[])
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_DISCOVER_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_DISCOVER_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_DISCOVER_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_DISCOVER_RESPONSE);
        return -1;
    }

    // parse out number of files
    *out_num_of_files = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // parse out each file
    for(int i = 0; i < *out_num_of_files; i++)
    {
        // file name
        strcpy(out_files[i].name, in_msg + MSG_HEADER_SIZE + bytes_read);
        bytes_read += strlen(out_files[i].name) + 1;
        
        // parse out file size
        out_files[i].size = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
        bytes_read += 4;

        // parse out check sum
        out_files[i].check_sum = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
        bytes_read += 4;
    }

    // success
    return 0;
}

int msg_Build_FindRequest(char* out_msg, const char* file_name)
{
    uint32_t bytes_written = 0;

    // write file name
    strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, file_name);
    bytes_written += strlen(file_name) + 1;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_FIND_REQUEST,  bytes_written);
    
    // success
    return 0;
}

int msg_Parse_FindRequest(const char* in_msg, char* out_file_name)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_FIND_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_FIND_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_FIND_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_FIND_REQUEST);
        return -1;
    }

    // parse out file name
    strcpy(out_file_name, in_msg + MSG_HEADER_SIZE + bytes_read);
    bytes_read += strlen(out_file_name) + 1;

    // success
    return 0;
}

int msg_Build_FindResponse(char* out_msg, int num_of_peers, const PeerInfo peers[])
{
    uint32_t bytes_written = 0;

    // write number of peers
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(num_of_peers);
    bytes_written += 4;

    // write each peer
    for(int i = 0; i < num_of_peers; i++)
    {
        // write peer address
        strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, peers[i].listening_addr);
        bytes_written += strlen(peers[i].listening_addr) + 1;
        
        // write peer port
        *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peers[i].listening_port);
        bytes_written += 4;

        // write latitude
        *(int32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peers[i].latitude);
        bytes_written += 4;

        // write longitude
        *(int32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peers[i].longitude);
        bytes_written += 4;
    }

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_FIND_RESPONSE,  bytes_written);
    
    // success
    return 0;
}

int msg_Parse_FindResponse(const char* in_msg, int* out_num_of_peers, PeerInfo out_peers[])
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_FIND_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_FIND_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_FIND_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_FIND_RESPONSE);
        return -1;
    }

    // parse out number of peers
    *out_num_of_peers = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // parse out each file
    for(int i = 0; i < *out_num_of_peers; i++)
    {
        // parse out peer address
        strcpy(out_peers[i].listening_addr, in_msg + MSG_HEADER_SIZE + bytes_read);
        bytes_read += strlen(out_peers[i].listening_addr) + 1;
        
        // parse out peer port
        out_peers[i].listening_port = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
        bytes_read += 4;

        // parse out latitude
        out_peers[i].latitude = ntohl(*(int32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
        bytes_read += 4;

        // parse out longitude
        out_peers[i].longitude = ntohl(*(int32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
        bytes_read += 4;
    }

    // success
    return 0;
}

int msg_Build_UpdateListRequest(char* out_msg, const PeerInfo* peer_info, int num_of_files, const FileInfo files[])
{
    uint32_t bytes_written = 0;

    // write peer address
    strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, peer_info->listening_addr);
    bytes_written += strlen(peer_info->listening_addr) + 1;

    // write peer port
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peer_info->listening_port);
    bytes_written += 4;

    // write latitude
    *(int32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peer_info->latitude);
    bytes_written += 4;

    // write longitude
    *(int32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peer_info->longitude);
    bytes_written += 4;

    // write peer port
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(num_of_files);
    bytes_written += 4;

    // write each file
    for(int i = 0; i < num_of_files; i++)
    {
        // write file name
        strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, files[i].name);
        bytes_written += strlen(files[i].name) + 1;
        
        // write file size
        *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(files[i].size);
        bytes_written += 4;

        // write check sum
        *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(files[i].check_sum);
        bytes_written += 4;
    }

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_UPDATE_LIST_REQUEST,  bytes_written);
    
    // success
    return 0;    
}

int msg_Parse_UpdateListRequest(const char* in_msg, PeerInfo* out_peer_info, int* out_num_of_files, FileInfo out_files[])
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_UPDATE_LIST_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_UPDATE_LIST_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_UPDATE_LIST_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_UPDATE_LIST_REQUEST);
        return -1;
    }

    // parse out peer address
    strcpy(out_peer_info->listening_addr, in_msg + MSG_HEADER_SIZE + bytes_read);
    bytes_read += strlen(out_peer_info->listening_addr) + 1;

    // parse out peer port
    out_peer_info->listening_port = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;    

    // parse out latitude
    out_peer_info->latitude = ntohl(*(int32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;  

    // parse out longitude
    out_peer_info->longitude = ntohl(*(int32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // parse out number of files
    *out_num_of_files = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // parse out each file
    for(int i = 0; i < *out_num_of_files; i++)
    {
        // parse out file name
        strcpy(out_files[i].name, in_msg + MSG_HEADER_SIZE + bytes_read);
        bytes_read += strlen(out_files[i].name) + 1;
        
        // parse out file size
        out_files[i].size = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
        bytes_read += 4;

        // parse out check sum
        out_files[i].check_sum = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
        bytes_read += 4;
    }

    // success
    return 0;
}

int msg_Build_UpdateListResponse(char* out_msg)
{
    // write header
    msg_Build_Header(out_msg, MSG_TYPE_UPDATE_LIST_RESPONSE,  0);
    
    // success
    return 0;
}

int msg_Build_PingRequest(char* out_msg, const PeerInfo* peer)
{
    uint32_t bytes_written = 0;

    // write peer address
    strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, peer->listening_addr);
    bytes_written += strlen(peer->listening_addr) + 1;

    // write peer port
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peer->listening_port);
    bytes_written += 4;

    // write latitude
    *(int32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peer->latitude);
    bytes_written += 4;

    // write longitude
    *(int32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(peer->longitude);
    bytes_written += 4;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_PING_REQUEST,  bytes_written);
    
    // success
    return 0;    
}

int msg_Parse_PingRequest(const char* in_msg, PeerInfo* out_peer)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_PING_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_PING_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_PING_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_PING_REQUEST);
        return -1;
    }

    // parse out peer address
    strcpy(out_peer->listening_addr, in_msg + MSG_HEADER_SIZE + bytes_read);
    bytes_read += strlen(out_peer->listening_addr) + 1;

    // parse out peer port
    out_peer->listening_port = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;    

    // parse out latitude
    out_peer->latitude = ntohl(*(int32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;  

    // parse out longitude
    out_peer->longitude = ntohl(*(int32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // success
    return 0;
}

int msg_Build_PingResponse(char* out_msg, int recognized)
{
    uint32_t bytes_written = 0;

    // write peer port
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(recognized);
    bytes_written += 4;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_PING_RESPONSE,  bytes_written);
    
    // success
    return 0;    
}

int msg_Parse_PingResponse(const char* in_msg, int* out_recognized)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_PING_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_PING_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_PING_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_PING_RESPONSE);
        return -1;
    }

    // parse out recognized
    *out_recognized = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // success
    return 0;    
}

int msg_Build_GetLoadRequest(char* out_msg)
{
    // write header
    msg_Build_Header(out_msg, MSG_TYPE_GET_LOAD_REQUEST,  0);
    
    // success
    return 0;
}

int msg_Build_GetLoadResponse(char* out_msg, int loads)
{
    uint32_t bytes_written = 0;

    // write loads
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(loads);
    bytes_written += 4;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_GET_LOAD_RESPONSE,  bytes_written);
    
    // success
    return 0;    
}

int msg_Parse_GetLoadResponse(const char* in_msg, int* out_loads)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_GET_LOAD_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_GET_LOAD_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_GET_LOAD_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_GET_LOAD_RESPONSE);
        return -1;
    }

    // parse out loads
    *out_loads = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // success
    return 0;    
}

int msg_Build_DownloadRequest(char* out_msg, const char* file_name)
{
    uint32_t bytes_written = 0;

    strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, file_name);
    bytes_written += strlen(file_name) + 1;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_DOWNLOAD_REQUEST,  bytes_written);
    
    // success
    return 0;    
}

int msg_Parse_DownloadRequest(const char* in_msg, char* out_file_name)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_DOWNLOAD_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_DOWNLOAD_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_DOWNLOAD_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_DOWNLOAD_REQUEST);
        return -1;
    }

    // parse out file name
    strcpy(out_file_name, in_msg + MSG_HEADER_SIZE + bytes_read);
    bytes_read += strlen(out_file_name) + 1;

    // success
    return 0;
}

int msg_Build_DownloadResponse(char* out_msg, const FileInfo* file_info, char* file_data)
{
    uint32_t bytes_written = 0;

    // write file name
    strcpy(out_msg + MSG_HEADER_SIZE + bytes_written, file_info->name);
    bytes_written += strlen(file_info->name) + 1;

    // write file size
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(file_info->size);
    bytes_written += 4;

    // write file size
    *(uint32_t*)(out_msg + MSG_HEADER_SIZE + bytes_written) = htonl(file_info->check_sum);
    bytes_written += 4;

    // write file data
    memcpy(out_msg + MSG_HEADER_SIZE + bytes_written, file_data, file_info->size);
    bytes_written += file_info->size;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_DOWNLOAD_RESPONSE,  bytes_written);
    
    // success
    return 0;  
}

int msg_Parse_DownloadResponse(const char* in_msg, FileInfo* out_file_info, char* out_file_data)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_DOWNLOAD_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_DOWNLOAD_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_DOWNLOAD_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_DOWNLOAD_RESPONSE);
        return -1;
    }

    // parse out file name
    strcpy(out_file_info->name, in_msg + MSG_HEADER_SIZE + bytes_read);
    bytes_read += strlen(out_file_info->name) + 1;

    // parse out file size
    out_file_info->size = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // parse out check sum
    out_file_info->check_sum = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_SIZE + bytes_read));
    bytes_read += 4;

    // parse out file data
    memcpy(out_file_data, in_msg + MSG_HEADER_SIZE + bytes_read, out_file_info->size);
    bytes_read += 4;

    // success
    return 0;
}


//#define MSG_TYPE_DOWNLOAD_RESPONSE          0x7001
//// message structure:
//// - header (see structure above)
//// - file name (null terminated string)
//// - file size (uint32 4-bytes)
//// - file check sum (uint32 4-bytes)
//// - file data (raw bytes, corresponding to size in previous "file size" field)





//