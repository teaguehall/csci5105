#include "net.h"
#include <stdio.h>

int net_Post(ServerInfo server, char* author, char* title, char* contents)
{
    printf("Hello from clientNet_Post\n");

    return 0;
}

int net_Read(ServerInfo server, int max_article_count, int* out_article_count,  Article* out_data)
{
    return 0;
}

int net_Choose(ServerInfo server, int article_id, Article* out_data)
{
    return 0;
}

int net_Reply(ServerInfo server, int article, char* author, char* contents)
{
    return 0;
}
