#include "database.h"
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>



ArticleNode db[MAX_ARTICLES];
pthread_mutex_t db_lock = PTHREAD_MUTEX_INITIALIZER;
static int db_count = 0; // current number of articles

// posts article to database. returns the article ID if successful, otherwise returns -1
int db_Post(char* author, char* title, char* contents)
{
    int error = 0;

    // lock mutex
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // make sure database isn't full
    if(db_count >= MAX_ARTICLES)
    {
        error = -1;
        goto exit;
    }

    // build article node
    ArticleNode node;

    node.article.id = db_count++;
    node.article.parent_id = -1; // -1 indicates no parent
    node.article.depth = 0;
    strcpy(node.article.author, author);
    strcpy(node.article.title, title);
    strcpy(node.article.contents, contents);
    node.next = -1; // indicate this is the last article

    // find the previous "last article" and set it to point at the new article
    for(int i = 0; i < db_count; i++)
    {
        if(db[i].next == -1)
        {
            db[i].next = node.article.id;
            break;
        }
    }


    // copy article into array
    db[node.article.id] = node;
    
    // unlock mutex
    exit:
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // return error or article ID
    if(error)
        return error;
    else
        return node.article.id;
}

// outputs database articles in sorted order. return 0 on success, -1 on error 
int db_Read(int* out_count, Article* out_articles)
{
    int next = 0;

    // clear the number of articles outputted
    *out_count = 0;

    // lock mutex
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // write article outputs in sorted order
    for(int i = 0; i < db_count; i++)
    {
        memcpy(out_articles + (*out_count)++, &(db[next].article), sizeof(db[next].article));
        
        next = db[next].next; // update read position to next
    }
    
    // unlock mutex
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // return error or article ID
    return 0;
}


int db_Reply(int parent_id, char* author, char* contents);      // will return article IF if successlfu, -1 otherwise for error
int db_Choose(int article_id, Article* out_article);            // will return 0 on success, -1 otherwise for error
int db_Read(int* out_count, Article* out_articles);             // will return 0 on success, -1 otherwise for error