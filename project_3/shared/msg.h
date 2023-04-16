#ifndef MSG_H
#define MSG_H

#include "peer_info.h"
#include "file_info.h"

#include <stdlib.h>
#include <stdint.h>

// utils
size_t msg_GetActualSize(const char* msg);
int msg_Build_Header(char* out_msg, uint32_t type, uint32_t size);
int msg_Parse_Header(const char* in_msg, uint32_t* out_type, int32_t* out_id, uint32_t* out_size);

// common messages
int msg_Build_ErrorResponse(char* out_msg, char* in_err_string);
int msg_Parse_ErrorResponse(const char* in_msg, char* out_error_msg);

// peer-to-server messages
int msg_Build_FindRequest(char* out_msg, const char* file_name);
int msg_Parse_FindRequest(const char* in_msg, char* out_file_name);
int msg_Build_FindResponse(char* out_msg, int num_of_nodes, const PeerInfo nodes[]);
int msg_Parse_FindResponse(const char* in_msg, int* out_num_of_nodes, PeerInfo out_nodes[]);
int msg_Build_UpdateListRequest(char* out_msg, const PeerInfo* peer_info, int num_of_files, const FileInfo files[]);
int msg_Parse_UpdateListRequest(const char* in_msg, PeerInfo* out_peer_info, int* out_num_of_files, const FileInfo out_files[]);
int msg_Build_UpdateListResponse(char* out_msg);
int msg_Build_PingRequest(char* out_msg, PeerInfo peer);
int msg_Parse_PingRequest(const char* in_msg, PeerInfo* out_peer);
int msg_Build_PingRecognizedResponse(char* out_msg);
int msg_Build_PingUnRecognizedResponse(char* out_msg);

// peer-to-peer messages
int msg_Build_GetLoadRequest(char* out_msg);
int msg_Build_GetLoadResponse(char* out_msg, int loads);
int msg_Parse_GetLoadRequest(const char* in_msg, int* out_loads);
int msg_Build_DownloadRequest(char* out_msg, const char* file_name);
int msg_Parse_DownloadRequest(const char* in_msg, char* out_file_name);
int msg_Build_DownloadResponse(char* out_msg, const FileInfo* file_info, char* file_data);
int msg_Parse_DownloadResponse(const char* in_msg, FileInfo* out_file_info, char* out_file_data);

#define MSG_MAGIC_NUMBER                    0x12AB34CD      // ol' magic
#define MSG_HEADER_SIZE                     16              // bytes

// message header:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message ID (uint32 4-bytes)
// - message size (uint32 4-bytes)

#define MSG_TYPE_ERROR_RESPONSE             0x1000
// message structure:
// - header (see structure above)
// - error message (null terminated string)

#define MSG_TYPE_FIND_REQUEST               0x2000
// message structure:
// - header (see structure above)
// - file name (null terminated string)

#define MSG_TYPE_FIND_RESPONSE              0x2001
// message structure:
// - header (see structure above)
// - number of nodes (uint32 4-bytes)
//      - node address (null terminated string)
//      - node port (uint32 4-bytes)
//      - latitude (int32 4-bytes)
//      - longitude (int32 4-bytes)

#define MSG_TYPE_UPDATE_LIST_REQUEST        0x3000
// message structure:
// - header (see structure above)
// - peer address (null terminated string)
// - peer port (uint32 4-bytes)
// - peer latitude (int32 4-bytes)
// - peer longitude (int32 4-bytes)
// - number of files (uint32 4-bytes)
//      - file name (null terminated string)
//      - file size (uint32 4-bytes)
//      - file check sum (uint32 4-bytes)

#define MSG_TYPE_UPDATE_LIST_RESPONSE       0x3001
// message structure:
// - header (see structure above)

#define MSG_TYPE_PING_REQUEST               0x4000
// message structure:
// - header (see structure above)

#define MSG_TYPE_PING_RESPONSE_RECOGNIZED   0x4001
// message structure:
// - header (see structure above)

#define MSG_TYPE_PING_RESPONSE_UNRECOGNIZED 0x4002
// message structure:
// - header (see structure above)

#define MSG_TYPE_GET_LOAD_REQUEST           0x5000
// message structure:
// - header (see structure above)

#define MSG_TYPE_GET_LOAD_RESPONSE          0x5001
// message structure:
// - header (see structure above)
// - loads (uint32 4-bytes)

#define MSG_TYPE_DOWNLOAD_REQUEST           0x6000
// message structure:
// - header (see structure above)
// - file name (null terminated string)

#define MSG_TYPE_DOWNLOAD_RESPONSE          0x6001
// message structure:
// - header (see structure above)
// - file name (null terminated string)
// - file size (uint32 4-bytes)
// - file check sum (uint32 4-bytes)
// - file data (raw bytes, corresponding to size in previous "file size" field)

#endif // MSG_H
