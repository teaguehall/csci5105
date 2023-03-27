#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include "../shared/article.h"
#include "database.h"

int net_Post(char* address, int port, char* author, char* title, char* contents);
int net_Read(char* address, int port, int max_article_count, unsigned int* out_article_count, Article out_articles[]);
int net_Choose(char* address, int port, int article_id, Article* out_article);
int net_Reply(char* address, int port, int article, char* author, char* contents);
int net_DbPush(char* address, int port, ArticleDatabase* database);
int net_DbPull(char* address, int port, ArticleDatabase* out_database);

#endif // CLIENT_NET_H
