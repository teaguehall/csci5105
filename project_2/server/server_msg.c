
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include "../shared/msg.h"
#include "server_msg.h"
#include "database.h"

// Builds MSG_TYPE_DB_PUSH_REQUEST message. Returns 0 on success, -1 on error
int msg_Build_DbPushRequest(char* out_msg, ArticleDatabase* in_db)
{
    uint32_t bytes_written = 0;

    // write db node count
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(in_db->article_count);
    bytes_written += 4;

    // write each db node
    for(int i = 0; i < in_db->article_count; i++)
    {
        // write next db node
        *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(in_db->nodes[i].next);
        bytes_written += 4;

        // write article ID
        *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(in_db->nodes[i].article.id);
        bytes_written += 4;

        // write parent article ID
        *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(in_db->nodes[i].article.parent_id);
        bytes_written += 4;

        // write article depth
        *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(in_db->nodes[i].article.depth);
        bytes_written += 4;

        // write article author
        strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, in_db->nodes[i].article.author);
        bytes_written += strlen(in_db->nodes[i].article.author) + 1;

        // write article title
        strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, in_db->nodes[i].article.title);
        bytes_written += strlen(in_db->nodes[i].article.title) + 1;

        // write article contents
        strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, in_db->nodes[i].article.contents);
        bytes_written += strlen(in_db->nodes[i].article.contents) + 1;

    }

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_DB_PUSH_REQUEST,  bytes_written);
    
    // success
    return 0;   
}

// Parses MSG_TYPE_DB_PUSH_REQUEST message. Returns 0 on success, -1 on error
int msg_Parse_DbPushRequest(char* in_msg, ArticleDatabase* out_db)
{
    uint32_t msg_type, msg_size;
    int32_t msg_id;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_id, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_DB_PUSH_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_DB_PUSH_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_DB_PUSH_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_DB_PUSH_REQUEST);
        return -1;
    }

    // parse out db version
    out_db->version = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;

    // parse out number of db nodes
    out_db->article_count = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;

    // parse out each article
    for(int i = 0; i < out_db->article_count; i++)
    {
        // parse out next node
        out_db->nodes[i].next = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
        bytes_read += 4;
        
        // parse out article ID
        out_db->nodes[i].article.id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
        bytes_read += 4;

        // parse out parent article ID
        out_db->nodes[i].article.parent_id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
        bytes_read += 4;

        // parse out article depth
        out_db->nodes[i].article.depth = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
        bytes_read += 4;

        // parse out article author
        strcpy(out_db->nodes[i].article.author, in_msg + MSG_HEADER_OFFSET + bytes_read);
        bytes_read += strlen(out_db->nodes[i].article.author) + 1;

        // parse out article title
        strcpy(out_db->nodes[i].article.title, in_msg + MSG_HEADER_OFFSET + bytes_read);
        bytes_read += strlen(out_db->nodes[i].article.title) + 1;

        // parse out article contents
        strcpy(out_db->nodes[i].article.contents, in_msg + MSG_HEADER_OFFSET + bytes_read);
        bytes_read += strlen(out_db->nodes[i].article.contents) + 1;
    }

    // success
    return 0;
}
