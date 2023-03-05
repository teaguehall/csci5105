#include "msg.h"
#include <string.h>
#include <arpa/inet.h>

// Builds message header, always succeeds and returns 0
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

// Parses out header info from raw message. Returns 0 on success, -1 on error
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

// Parses out ERROR response from raw message
int msg_Parse_ErrorResponse(char* in_msg, char* out_error_msg)
{
    uint32_t msg_type, msg_size;
    
    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing Error Response message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_ERROR_RESPONSE)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing Error Response message. Found %d, Expected %d\n", msg_type, MSG_TYPE_ERROR_RESPONSE);
        return -1;
    }

    // copy string to output
    strcpy(out_error_msg, in_msg + MSG_HEADER_OFFSET);

    // success
    return 0;
}

// Builds POST request to raw message
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

// Parses out POST REQUEST info from raw message
int msg_Parse_PostRequest(char* in_msg, char* out_author, char* out_title, char* out_contents)
{
    uint32_t msg_type, msg_size;
    uint32_t bytes_read = 0;

    // validate header
    if(msg_Parse_Header(in_msg, &msg_type, &msg_size))
    {
        fprintf(stderr, "ERROR: Invalid header found while parsing POST REQUEST message\n");
        return -1;
    }
    
    // validate type
    if(msg_type != MSG_TYPE_POST_REQUEST)
    {
        fprintf(stderr, "ERROR: Incorrect message type while parsing POST REQUEST message. Found %d, Expected %d\n", msg_type, MSG_TYPE_POST_REQUEST);
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

    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)
    // - page size (uint32 4-bytes)
    // - page number (uint32 4-bytes)

// Builds read request message
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

// Parses out READ REQUEST info from raw message 
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

// Builds MSG_TYPE_READ_RESPONSE message
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


    // message structure:
    // - magic number (uint32 4-bytes)
    // - message type (uint32 4-bytes)
    // - message size (uint32 4-bytes)
    // - number of articles (uint32 4-bytes)
    //      - article ID (uint32 4-bytes)
    //      - parent article ID (uint32 4-bytes)
    //      - article author (null-terminated string)
    //      - article title (null-terminated string)
    //      - article contents (null-terminated string)


// Parses out READ response info from raw message
int msg_Parse_ReadResponse(char* in_msg, uint32_t* out_article_count, Article* out_articles)
{
    // TODO

    // success
    return 0;
}

// Parses out CHOOSE request info from raw message
int msg_Parse_ChooseRequest(char* in_msg, uint32_t* out_article_id)
{
    // TODO

    // success
    return 0;
}

// Parses out CHOOSE response info from raw message
int msg_Parse_ChooseResponse(char* in_msg, Article* out_articles)
{
    // TODO

    // success
    return 0;
}

// Parses out REPLAY request from raw message
int msg_Parse_ReplyRequest(char* in_msg, uint32_t* out_article_id, char* out_author, char* out_contents)
{
    // TODO

    // success
    return 0;
}



// Builds ERROR response message to raw output message
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



int msg_Build_PostResponse(char* out_msg)
{
    // TODO

    // success
    return 0;
}





int msg_Build_ChooseRequest(char* out_msg, uint32_t article_id)
{
    // TODO

    // success
    return 0;
}

int msg_Build_ChooseResponse(char* out_msg, Article articles)
{
    // TODO

    // success
    return 0;
}

int msg_Build_ReplyRequest(char* out_msg, uint32_t article_id, char* author, char* contents)
{
    // TODO

    // success
    return 0;
}

int msg_Build_ReplyResponse(char* out_msg)
{
    // TODO

    // success
    return 0;
}
