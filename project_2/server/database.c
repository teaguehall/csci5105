#include "database.h"



// databse is linked list of article nodes
typedef struct CoordinatorInfo
{
    char coord_address[128];
    int coord_port;

} CoordinatorInfo;

int db_Post(char* author, char* title, char* contents);         // will return article ID if successful, -1 otherwise for error
int db_Reply(int parent_id, char* author, char* contents);      // will return article IF if successlfu, -1 otherwise for error
int db_Choose(int article_id, Article* out_article);            // will return 0 on success, -1 otherwise for error
int db_Read(int* out_count, Article* out_articles);             // will return 0 on success, -1 otherwise for error