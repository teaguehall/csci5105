#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#include "article.h"

typedef struct ConnectInfo {
    char hostname[1024];
    int port;
} ConnectInfo;

int clientNet_Connect();
int clientNet_Post(char* author, char* title, char* contents);
int clientNet_Read(int max_article_count, int* out_article_count,  Article* out_data);
int clientNet_Choose(int article_id, Article* out_data);
int clientNet_Reply(int article, char* author, char* contents);

#endif // CLIENT_NET_H
