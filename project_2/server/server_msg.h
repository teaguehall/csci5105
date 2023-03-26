#ifndef SERVER_MSG_H
#define SERVER_MSG_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <stdint.h>
    #include "../shared/msg.h"
    #include "../shared/article.h"
    #include "../shared/database.h"

    #define MAX_SERVER_MSG_SIZE (sizeof(ArticleDatabase) + MSG_HEADER_OFFSET) // the biggest server message encapsulates an entire database...

    int msg_Build_DbPushRequest(char* out_msg, ArticleDatabase* in_db);
    int msg_Parse_DbPushRequest(char* in_msg, ArticleDatabase* out_db);
    int msg_Build_DbPushResponse(char* out_msg);



#endif // SERVER_MSG_H
