#include "database.h"
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>



ArticleNode db[MAX_ARTICLES];
pthread_mutex_t db_lock = PTHREAD_MUTEX_INITIALIZER;
static int db_count = 0; // current number of articles

// posts article to database. returns the article ID if successful, otherwise returns -1
int db_Post(char* author, char* title, char* contents, int* out_err_db_full)
{
    int error = 0;
    *out_err_db_full = 0;

    // lock mutex
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // make sure database isn't full
    if(db_count >= MAX_ARTICLES)
    {
        fprintf(stderr, "ERROR: Client attempted to post article to database that is full.\n");
        *out_err_db_full = 1;
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
        memcpy(out_articles + i, &(db[next].article), sizeof(db[next].article));
        
        next = db[next].next; // update read position to next
        (*out_count)++;
    }
    
    // unlock mutex
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // return error or article ID
    return 0;
}

// submits a reply to the database. returns article ID on success, other -1 on error
int db_Reply(int parent_id, char* author, char* contents, int* out_err_db_full, int* out_err_invalid_id)
{
    char title[ARTICLE_MAX_TITLE];
    int i, saved_next;
    int parent_index = -1;
    int child_index = -1;
    int error = 0;

    // clear error flags
    *out_err_db_full = 0;
    *out_err_invalid_id = 0;

    // lock mutex
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // make sure database isn't full
    if(db_count >= MAX_ARTICLES)
    {
        fprintf(stderr, "ERROR: Client attempted to reply to article when database is already full.\n");
        *out_err_db_full = 1;
        error = -1;
        goto exit;
    }

    // verify parent id is present in database
    for(i = 0; i < db_count; i++)
    {
        if(db[i].article.id == parent_id)
        {
            parent_index = i;
            break;
        }
    }
    if(parent_index == -1)
    {
        fprintf(stderr, "ERROR: Client attempted to reply to non-existent article %d.\n", parent_id);
        *out_err_invalid_id =1;
        error = -1;
        goto exit;
    }

    // build response title 
    sprintf(title, "RE: %s", db[parent_index].article.title);

    // build article node
    ArticleNode node;

    node.article.id = db_count;
    node.article.parent_id = parent_id;
    node.article.depth = db[parent_index].article.depth + 1;
    strcpy(node.article.author, author);
    strcpy(node.article.title, title);
    strcpy(node.article.contents, contents);

    // scan database backward to check if there are any other child responses to the parent id already
    for(i = (db_count - 1); i >= 0; i--)
    {        
        if(db[i].article.parent_id == parent_id)
        {
            child_index = i;
            break;
        }
    }

    // update node nexts
    if(child_index != -1) // indicates there is already a response to what we're responding to
    {
        printf("Previous reply found\n");
        saved_next = db[child_index].next;
        db[child_index].next = node.article.id;
        node.next = saved_next;
    }
    else // indicates were the first response
    {
        printf("No previous reply found\n");
        saved_next = db[parent_index].next;
        db[parent_index].next = node.article.id;
        node.next = saved_next;
    }

    // copy article into array
    db[db_count++] = node;

    // unlock mutex
    exit:
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // return error or article ID
    if(error)
        return error;
    else
        return node.article.id;
}

// chooses article from database. returns 0 on success, -1 on error
int db_Choose(int article_id, Article* out_article, int* out_err_invalid_id)
{
    int error = 0;
    int found = 0;
    *out_err_invalid_id = 0;

    // lock mutex
    if(pthread_mutex_lock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // find article
    for(int i = 0; i < db_count; i++)
    {
        if(db[i].article.id == article_id)
        {
            *out_article = db[i].article;
            found = 1;
            break;
        }
    }

    // throw error if not found
    if(!found)
    {
        *out_err_invalid_id =1;
        error = -1;
        goto exit;
    }
    
    // unlock mutex
    exit:
    if(pthread_mutex_unlock(&db_lock) != 0) {fprintf(stderr, "ERROR: Failed to lock article database mutex. %s", strerror(errno)); return -1;}

    // return error or article ID
    if(error)
        return error;
    else
        return 0;
}