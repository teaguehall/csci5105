#ifndef MSG_H
#define MSG_H

#include "peer_info.h"

#include <stdlib.h>
#include <stdint.h>

// utils
size_t msg_GetActualSize(const char* msg);
int msg_Build_Header(char* out_msg, uint32_t type, uint32_t size);
int msg_Parse_Header(const char* in_msg, uint32_t* out_type, int32_t* out_id, uint32_t* out_size);

// common messages
int msg_Build_ErrorResponse(char* out_msg, char* in_err_string);
int msg_Parse_ErrorResponse(char* in_msg, char* out_error_msg);

// peer-to-server messages
int msg_Build_FindRequest(char* out_msg, const char* file_name);
int msg_Parse_FindRequest(const char* in_msg, char* out_file_name);
int msg_Build_FindResponse(char* out_msg, int num_of_nodes, const PeerInfo nodes[]);
int msg_Parse_FindResponse(const char* in_msg, int* out_num_of_nodes, PeerInfo out_nodes[]);


// peer-to-peer messages
// TODO 

#define MSG_MAGIC_NUMBER                0x12AB34CD
#define MSG_HEADER_SIZE                 16              // bytes

#define MSG_TYPE_ERROR_RESPONSE         0x1000
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - error message (null terminated string)

#define MSG_TYPE_POST_REQUEST           0x2000
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - post author (null terminated string)
// - post title (null terminated string)
// - post contents (null terminated string)

#define MSG_TYPE_POST_RESPONSE          0x2001
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)

#define MSG_TYPE_READ_REQUEST           0x3000
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - page size (uint32 4-bytes)
// - page number (uint32 4-bytes)

#define MSG_TYPE_READ_RESPONSE          0x3001
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - number of articles (uint32 4-bytes)
//      - article ID (uint32 4-bytes)
//      - parent article ID (uint32 4-bytes)
//      - article depth (uint32 4-bytes)
//      - article author (null-terminated string)
//      - article title (null-terminated string)
//      - article contents (null-terminated string)

#define MSG_TYPE_CHOOSE_REQUEST         0x4000
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - article ID (uint32 4-bytes)

#define MSG_TYPE_CHOOSE_RESPONSE        0x4001
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - article ID (uint32 4-bytes)
// - parent article ID (uint32 4-bytes)
// - article author (null-terminated string)
// - article title (null-terminated string)
// - article contents (null-terminated string)

#define MSG_TYPE_REPLY_REQUEST          0x5000
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - article ID to reply (uint32 4-bytes)
// - post author (null terminated string)
// - post contents (null terminated string)

#define MSG_TYPE_REPLY_RESPONSE         0x5001
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)

#define MSG_TYPE_DB_PUSH_REQUEST        0x6000
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - db version (uint32 4-bytes)
// - number of db nodes (uint32 4-bytes)
//      - next article (uint32 4-bytes)
//      - article ID (uint32 4-bytes)
//      - parent article ID (uint32 4-bytes)
//      - article depth (uint32 4-bytes)
//      - article author (null-terminated string)
//      - article title (null-terminated string)
//      - article contents (null-terminated string)

#define MSG_TYPE_DB_PUSH_RESPONSE       0x6001
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)

#define MSG_TYPE_DB_PULL_REQUEST        0x7000
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)

#define MSG_TYPE_DB_PULL_RESPONSE       0x7001
// message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - db version (uint32 4-bytes)
// - number of db nodes (uint32 4-bytes)
//      - next article (uint32 4-bytes)
//      - article ID (uint32 4-bytes)
//      - parent article ID (uint32 4-bytes)
//      - article depth (uint32 4-bytes)
//      - article author (null-terminated string)
//      - article title (null-terminated string)
//      - article contents (null-terminated string)

#endif // MSG_H
