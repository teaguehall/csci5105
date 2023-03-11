#ifndef PROTOCOL_H
#define PROTOCOL_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include "article.h"

    size_t msg_GetActualSize(char* msg);

    // message parsing and building functions
    int msg_Build_Header(char* out_msg, uint32_t type,  uint32_t size);
    int msg_Parse_Header(char* in_msg, uint32_t* out_type, uint32_t* out_size);
    int msg_Build_ErrorResponse(char* out_msg, char* in_err_string);
    int msg_Parse_ErrorResponse(char* in_msg, char* out_error_msg);
    int msg_Build_PostRequest(char* out_msg, char* in_author, char* in_title, char* in_contents);
    int msg_Parse_PostRequest(char* in_msg, char* out_author, char* out_title, char* out_contents);
    int msg_Build_PostResponse(char* out_msg);
    int msg_Build_ReadRequest(char* out_msg, uint32_t max_articles);
    int msg_Parse_ReadRequest(char* in_msg, uint32_t* out_max_articles);
    int msg_Build_ReadResponse(char* out_msg, int article_count, Article articles[]);
    int msg_Parse_ReadResponse(char* in_msg, uint32_t* out_article_count, Article out_articles[]);
    int msg_Build_ChooseRequest(char* out_msg, uint32_t article_id);
    int msg_Parse_ChooseRequest(char* in_msg, uint32_t* out_article_id);
    int msg_Build_ChooseResponse(char* out_msg, Article article);
    int msg_Parse_ChooseResponse(char* in_msg, Article* out_articles);
    int msg_Build_ReplyRequest(char* out_msg, uint32_t article_id, char* author, char* contents);
    int msg_Parse_ReplyRequest(char* in_msg, uint32_t* out_article_id, char* out_author, char* out_contents);
    int msg_Build_ReplyResponse(char* out_msg);
   
    #define MSG_MAGIC_NUMBER                0x12AB34CD
    #define MSG_HEADER_OFFSET               12

    #define MSG_TYPE_ERROR_RESPONSE         0x1000

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)
    // - error message (null terminated string)

    #define MSG_TYPE_POST_REQUEST           0x2000

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)
    // - post author (null terminated string)
    // - post title (null terminated string)
    // - post contents (null terminated string)

    #define MSG_TYPE_POST_RESPONSE          0x2001

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)

    #define MSG_TYPE_READ_REQUEST           0x3000

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)
    // - page size (uint32 4-bytes)
    // - page number (uint32 4-bytes)

    #define MSG_TYPE_READ_RESPONSE          0x3001

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)
    // - number of articles (uint32 4-bytes)
    //      - article ID (uint32 4-bytes)
    //      - parent article ID (uint32 4-bytes)
    //      - article author (null-terminated string)
    //      - article title (null-terminated string)
    //      - article contents (null-terminated string)

    #define MSG_TYPE_CHOOSE_REQUEST         0x4000

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)
    // - article ID (uint32 4-bytes)

    #define MSG_TYPE_CHOOSE_RESPONSE        0x4001

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
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
    // - message size (uint32 4-bytes)
    // - article ID to reply (uint32 4-bytes)
    // - post author (null terminated string)
    // - post contents (null terminated string)

    #define MSG_TYPE_REPLY_RESPONSE         0x5001

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)



#endif // PROTOCOL_H
