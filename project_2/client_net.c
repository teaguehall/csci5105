#include "client_net.h"
#include <stdio.h>

int clientNet_Post(ConnectInfo connect_info, char* author, char* title, char* contents)
{
    printf("Hello from clientNet_Post\n");

    return 0;
}

int clientNet_Read(ConnectInfo connect_info, int max_article_count, int* out_article_count,  Article* out_data)
{
    return 0;
}

int clientNet_Choose(ConnectInfo connect_info, int article_id, Article* out_data)
{
    return 0;
}

int clientNet_Reply(ConnectInfo connect_info, int article, char* author, char* contents)
{
    return 0;
}