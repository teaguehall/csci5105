#ifndef SERVER_MSG_H
#define SERVER_MSG_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include "../shared/msg.h"
    #include "../shared/article.h"
    #include "database.h"

    int msg_Build_DbPushRequest(char* out_msg, ArticleDatabase* in_db);
    int msg_Parse_DbPushRequest(char* in_msg, ArticleDatabase* out_db);
    int msg_Build_DbPushResponse(char* out_msg);

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

#endif // SERVER_MSG_H
