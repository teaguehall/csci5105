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
int msg_Build_Post(char* out_msg, char* in_author, char* in_title, char* in_contents)
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
    msg_Build_Header(out_msg, MSG_TYPE_POST,  bytes_written);
    
    // success
    return 0;
}

// Parses out POST request info from raw message
int msg_Parse_Post(char* in_msg, char* out_author, char* out_title, char* out_contents)
{
    // TODO

    // success
    return 0;
}

// Parses out READ request info from raw message 
int msg_Parse_Read(char* in_msg, uint32_t* out_page_size, uint32_t* out_page_number)
{
    // TODO

    // success
    return 0;
}

// Parses out READ response info from raw message
int msg_Parse_ReadResponse(char* in_msg, uint32_t* out_article_count, Article* out_articles)
{
    // TODO

    // success
    return 0;
}

// Parses out CHOOSE request info from raw message
int msg_Parse_Choose(char* in_msg, uint32_t* out_article_id)
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
int msg_Parse_Reply(char* in_msg, uint32_t* out_article_id, char* out_author, char* out_contents)
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

int msg_Build_Read(char* out_msg, uint32_t page_size, uint32_t page_number)
{
    // TODO

    // success
    return 0;
}

int msg_Build_ReadResponse(char* out_msg, int article_count, Article articles[])
{
    // TODO

    // success
    return 0;
}

int msg_Build_Choose(char* out_msg, uint32_t article_id)
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

int msg_Build_Reply(char* out_msg, uint32_t article_id, char* author, char* contents)
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
