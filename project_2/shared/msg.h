#ifndef PROTOCOL_H
#define PROTOCOL_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include "article.h"
    #include "database.h"

    #define MAX_MSG_SIZE    (sizeof(ArticleDatabase) + MSG_HEADER_OFFSET) // the biggest server message encapsulates an entire database...

    // util functions
    size_t msg_GetActualSize(char* msg);
    int msg_Build_Header(char* out_msg, uint32_t type, uint32_t size);
    int msg_Parse_Header(char* in_msg, uint32_t* out_type, int32_t* out_id, uint32_t* out_size);
    
    // client-server AND server-server functions 
    int msg_Build_ErrorResponse(char* out_msg, char* in_err_string);
    int msg_Parse_ErrorResponse(char* in_msg, char* out_error_msg);
    int msg_Build_PostRequest(char* out_msg, char* in_author, char* in_title, char* in_contents);
    int msg_Parse_PostRequest(char* in_msg, char* out_author, char* out_title, char* out_contents);
    int msg_Build_PostResponse(char* out_msg);
    int msg_Build_ReplyRequest(char* out_msg, uint32_t article_id, char* author, char* contents);
    int msg_Parse_ReplyRequest(char* in_msg, uint32_t* out_article_id, char* out_author, char* out_contents);
    int msg_Build_ReplyResponse(char* out_msg);
    
    // client-server ONLY messages
    int msg_Build_ReadRequest(char* out_msg, uint32_t max_articles);
    int msg_Parse_ReadRequest(char* in_msg, uint32_t* out_max_articles);
    int msg_Build_ReadResponse(char* out_msg, int article_count, Article articles[]);
    int msg_Parse_ReadResponse(char* in_msg, uint32_t* out_article_count, Article out_articles[]);
    int msg_Build_ChooseRequest(char* out_msg, uint32_t article_id);
    int msg_Parse_ChooseRequest(char* in_msg, uint32_t* out_article_id);
    int msg_Build_ChooseResponse(char* out_msg, Article article);
    int msg_Parse_ChooseResponse(char* in_msg, Article* out_articles);

    // server-server ONLY messages
    int msg_Build_DbPushRequest(char* out_msg, ArticleDatabase* in_db);
    int msg_Parse_DbPushRequest(char* in_msg, ArticleDatabase* out_db);
    int msg_Build_DbPushResponse(char* out_msg);

    int msg_Build_DbPullRequest(char* out_msg);
    int msg_Build_DbPullResponse(char* out_msg, ArticleDatabase* in_db);
    int msg_Parse_DbPullResponse(char* in_msg, ArticleDatabase* out_db);


    #define MSG_MAGIC_NUMBER                0x12AB34CD
    #define MSG_HEADER_OFFSET               16

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

#endif // PROTOCOL_H
