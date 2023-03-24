#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "database.h"

int protocol_SendArticle();
int protocol_SendReply();
int protocol_SendDatabase(ArticleDatabase* db_to_send);

#endif // SERVER_PROTOCOL_H
