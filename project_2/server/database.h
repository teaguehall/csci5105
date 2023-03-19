#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include "../shared/article.h"

typedef struct ArticleNode
{
    Article article;
    int next;

} ArticleNode;

// client functions
int db_Post(char* author, char* title, char* contents);         // will return article ID if successful, -1 otherwise for error
int db_Reply(int parent_id, char* author, char* contents);      // will return article IF if successlfu, -1 otherwise for error
int db_Choose(int article_id, Article* out_article);            // will return 0 on success, -1 otherwise for error
int db_Read(int* out_count, Article* out_articles);             // will return 0 on success, -1 otherwise for error

// database sync functions
int db_SyncGet(ArticleNode *out_nodes);         // will return 0 on success, -1 otherwise for error
int db_SyncPut(ArticleNode *nodes);             // will

#endif // SERVER_DATABASE_H
