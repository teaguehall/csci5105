#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include "../shared/article.h"

typedef struct ArticleNode
{
    int next;
    Article article;
    
} ArticleNode;

typedef struct ArticleDatabase
{
    int version;
    int article_count;
    ArticleNode nodes[MAX_ARTICLES];

} ArticleDatabase;

int db_Post(char* author, char* title, char* contents, int* out_err_db_full);
int db_Reply(int parent_id, char* author, char* contents, int* out_err_db_full, int* out_err_invalid_id);
int db_Choose(int article_id, Article* out_article, int* out_err_invalid_id);
int db_Read(int* out_count, Article* out_articles);

int db_Backup(ArticleDatabase* out_database);
int db_Restore(ArticleDatabase* in_database);

#endif // SERVER_DATABASE_H
