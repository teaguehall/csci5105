#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include "../shared/article.h"

typedef struct ServerInfo {
    char address[1024];
    int port;
} ServerInfo;

int net_Post(ServerInfo server, char* author, char* title, char* contents);
int net_Read(ServerInfo server, int max_article_count, unsigned int* out_article_count, Article out_articles[]);
int net_Choose(ServerInfo server, int article_id, Article* out_article);
int net_Reply(ServerInfo server, int article, char* author, char* contents);

#endif // CLIENT_NET_H
