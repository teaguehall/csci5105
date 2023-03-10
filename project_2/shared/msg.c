#include "msg.h"
#include <string.h>
#include <arpa/inet.h>

// Builds message header. Always succeeds and returns 0
int msg_Build_Header(char* out_msg, uint32_t type,  uint32_t size)
{
    // write magic number
    (*(uint32_t*)out_msg) = htonl(MSG_MAGIC_NUMBER);

    // write message type
    (*(uint32_t*)(out_msg + 4)) = htonl(type);

    // write message size (excludes size of header itself)
    (*(uint32_t*)(out_msg + 8)) = htonl(size);

    // success
    return 0;
}

// Parse message header. Returns 0 on success, -1 on error
int msg_Parse_Header(char* in_msg, uint32_t* out_type, uint32_t* out_size)
{
    uint32_t magic = ntohl(*((uint32_t*)in_msg));
    *out_type = ntohl(*(uint32_t*)(in_msg + 4));
    *out_size = ntohl(*(uint32_t*)(in_msg + 8));

    // validate header
    if(magic != MSG_MAGIC_NUMBER)
    {
        fprintf(stderr, "ERROR: Invalid magic number in header. Found %d, expected %d\n", magic, MSG_MAGIC_NUMBER);
        return -1;
    }

    // success
    return 0;
}

// Builds MSG_TYPE_ERROR_RESPONSE message. Returns 0 on success, -1 on error
int msg_Build_ErrorResponse(char* out_msg, char* in_err_string)
{
    uint32_t bytes_written = 0;
    
    // copy error string to message
    strcpy(out_msg + MSG_HEADER_OFFSET, in_err_string);
    bytes_written += strlen(in_err_string) + 1;

    // write message header
    msg_Build_Header(out_msg, MSG_TYPE_ERROR_RESPONSE,  bytes_written);

    // success
    return 0;
}

// Parses MSG_TYPE_ERROR_RESPONSE message. Returns 0 on success, -1 on error
int msg_Parse_ErrorResponse(char* in_msg, char* out_error_msg)
{
    uint32_t msg_type, msg_size;
    
    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_ERROR_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_ERROR_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_ERROR_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_ERROR_RESPONSE);
        return -1;
    }

    // copy string to output
    strcpy(out_error_msg, in_msg + MSG_HEADER_OFFSET);

    // success
    return 0;
}

// Builds MSG_TYPE_POST_REQUEST message. Returns 0 on success, -1 on error
int msg_Build_PostRequest(char* out_msg, char* in_author, char* in_title, char* in_contents)
{
    uint32_t bytes_written = 0;

    // write author
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, in_author);
    bytes_written += strlen(in_author) + 1;

    // write title
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, in_title);
    bytes_written += strlen(in_title) + 1;

    // write contents
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, in_contents);
    bytes_written += strlen(in_contents) + 1;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_POST_REQUEST,  bytes_written);
    
    // success
    return 0;
}

// Parses MSG_TYPE_POST_REQUEST message. Returns 0 on success, -1 on error
int msg_Parse_PostRequest(char* in_msg, char* out_author, char* out_title, char* out_contents)
{
    uint32_t msg_type, msg_size;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_POST_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_POST_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_POST_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_POST_REQUEST);
        return -1;
    }

    // parse out author
    strcpy(out_author, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_author) + 1;

    // parse out title
    strcpy(out_title, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_title) + 1;

    // parse out contents
    strcpy(out_contents, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_contents) + 1;

    // success
    return 0;
}

int msg_Build_PostResponse(char* out_msg)
{
    // write header
    msg_Build_Header(out_msg, MSG_TYPE_POST_RESPONSE,  0);

    // success
    return 0;      
}

// Builds MSG_TYPE_READ_REQUEST message. Returns 0 on success, -1 on error
int msg_Build_ReadRequest(char* out_msg, uint32_t page_size, uint32_t page_number)
{
    uint32_t bytes_written = 0;

    // write page size
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(page_size);
    bytes_written += 4;

    // write page number
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(page_number);
    bytes_written += 4;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_READ_REQUEST,  bytes_written);
    
    // success
    return 0;    
}

// Parses MSG_TYPE_READ_REQUEST message. Returns 0 on success, -1 on error
int msg_Parse_ReadRequest(char* in_msg, uint32_t* out_page_size, uint32_t* out_page_number)
{
    uint32_t msg_type, msg_size;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_READ_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_READ_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_READ_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_READ_REQUEST);
        return -1;
    }

    // parse out page size
    *out_page_size = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;

    // parse out page number
    *out_page_number = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;

    // success
    return 0;
}

// Builds MSG_TYPE_READ_RESPONSE message. Returns 0 on success, -1 on error
int msg_Build_ReadResponse(char* out_msg, int article_count, Article articles[])
{
    uint32_t bytes_written = 0;

    // write article count
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(article_count);
    bytes_written += 4;

    // write each arcticle
    for(int i = 0; i < article_count; i++)
    {
        // write article ID
        *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(articles[i].id);
        bytes_written += 4;

        // write parent article ID
        *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(articles[i].parent_id);
        bytes_written += 4;

        // write article depth
        *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(articles[i].depth);
        bytes_written += 4;

        // write article author
        strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, articles[i].author);
        bytes_written += strlen(articles[i].author) + 1;

        // write article title
        strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, articles[i].title);
        bytes_written += strlen(articles[i].title) + 1;

        // write article contents
        strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, articles[i].contents);
        bytes_written += strlen(articles[i].contents) + 1;

    }

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_READ_RESPONSE,  bytes_written);
    
    // success
    return 0;   
}

// Parses MSG_TYPE_READ_RESPONSE message. Returns 0 on success, -1 on error
int msg_Parse_ReadResponse(char* in_msg, uint32_t* out_article_count, Article out_articles[])
{
    uint32_t msg_type, msg_size;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_READ_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_READ_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_READ_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_READ_RESPONSE);
        return -1;
    }

    // parse out article count
    *out_article_count = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;

    // parse out each article
    for(int i = 0; i < *out_article_count; i++)
    {
        // parse out article ID
        out_articles[i].id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
        bytes_read += 4;

        // parse out parent article ID
        out_articles[i].parent_id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
        bytes_read += 4;

        // parse out article depth
        out_articles[i].depth = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
        bytes_read += 4;

        // parse out article author
        strcpy(out_articles[i].author, in_msg + MSG_HEADER_OFFSET + bytes_read);
        bytes_read += strlen(out_articles[i].author) + 1;

        // parse out article title
        strcpy(out_articles[i].title, in_msg + MSG_HEADER_OFFSET + bytes_read);
        bytes_read += strlen(out_articles[i].title) + 1;

        // parse out article contents
        strcpy(out_articles[i].contents, in_msg + MSG_HEADER_OFFSET + bytes_read);
        bytes_read += strlen(out_articles[i].contents) + 1;
    }

    // success
    return 0;
}

// Builds MSG_TYPE_CHOOSE_REQUEST message. Returns 0 on success, -1 on error
int msg_Build_ChooseRequest(char* out_msg, uint32_t article_id)
{
    uint32_t bytes_written = 0;

    // write article count
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(article_id);
    bytes_written += 4;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_CHOOSE_REQUEST,  bytes_written);
    
    // success
    return 0;   
}

// Parses MSG_TYPE_CHOOSE_REQUEST message. Returns 0 on success, -1 on error
int msg_Parse_ChooseRequest(char* in_msg, uint32_t* out_article_id)
{
    uint32_t msg_type, msg_size;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_CHOOSE_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_CHOOSE_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_CHOOSE_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_CHOOSE_REQUEST);
        return -1;
    }

    // parse out article count
    *out_article_id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;

    // success
    return 0;
}

// Builds MSG_TYPE_CHOOSE_RESPONSE message. Returns 0 on success, -1 on error
int msg_Build_ChooseResponse(char* out_msg, Article article)
{
    uint32_t bytes_written = 0;

    // write article ID
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(article.id);
    bytes_written += 4;

    // write parent article ID
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(article.parent_id);
    bytes_written += 4;

    // write article depth
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(article.depth);
    bytes_written += 4;

    // write article author
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, article.author);
    bytes_written += strlen(article.author) + 1;

    // write article title
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, article.title);
    bytes_written += strlen(article.title) + 1;

    // write article contents
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, article.contents);
    bytes_written += strlen(article.contents) + 1;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_CHOOSE_RESPONSE,  bytes_written);
    
    // success
    return 0;   
}

// Parses MSG_TYPE_CHOOSE_RESPONSE message. Returns 0 on success, -1 on error
int msg_Parse_ChooseResponse(char* in_msg, Article* out_article)
{
    uint32_t msg_type, msg_size;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_CHOOSE_RESPONSE message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_CHOOSE_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_CHOOSE_RESPONSE message. Found %d, Expected %d\n", msg_type, MSG_TYPE_CHOOSE_RESPONSE);
        return -1;
    }

    // parse out article ID
    out_article->id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;  

    // parse out parent article ID
    out_article->parent_id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;    

    // parse out parent article depth
    out_article->depth = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4; 

    // parse out article author
    strcpy(out_article->author, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_article->author) + 1;   

    // parse out article title
    strcpy(out_article->title, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_article->title) + 1;   

    // parse out article contents
    strcpy(out_article->contents, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_article->contents) + 1;

    // success
    return 0;
}

// Builds MSG_TYPE_REPLY_REQUEST message. Returns 0 on success, -1 on error
int msg_Build_ReplyRequest(char* out_msg, uint32_t article_id, char* author, char* contents)
{
    uint32_t bytes_written = 0;

    // write article ID to which is being replied
    *(uint32_t*)(out_msg + MSG_HEADER_OFFSET + bytes_written) = htonl(article_id);
    bytes_written += 4;

    // write author of replier
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, author);
    bytes_written += strlen(author) + 1;

    // write contents of reply
    strcpy(out_msg + MSG_HEADER_OFFSET + bytes_written, contents);
    bytes_written += strlen(contents) + 1;

    // write header
    msg_Build_Header(out_msg, MSG_TYPE_REPLY_REQUEST,  bytes_written);
    
    // success
    return 0;
}

// Parses MSG_TYPE_REPLY_REQUEST message. Returns 0 on success, -1 on error
int msg_Parse_ReplyRequest(char* in_msg, uint32_t* out_article_id, char* out_author, char* out_contents)
{
    uint32_t msg_type, msg_size;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing MSG_TYPE_REPLY_REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_REPLY_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing MSG_TYPE_REPLY_REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_REPLY_REQUEST);
        return -1;
    }

    // parse out article ID
    *out_article_id = ntohl(*(uint32_t*)(in_msg + MSG_HEADER_OFFSET + bytes_read));
    bytes_read += 4;  

    // parse out author
    strcpy(out_author, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_author) + 1;

    // parse out contents
    strcpy(out_contents, in_msg + MSG_HEADER_OFFSET + bytes_read);
    bytes_read += strlen(out_contents) + 1; 

    // success
    return 0;
}
