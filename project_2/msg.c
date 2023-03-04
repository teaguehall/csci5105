#include "msg.h"

///

int msg_Parse_Header(char* in_msg, uint32_t* out_type, uint32_t* out_size)
{
}

int msg_Parse_ErrorResponse(char* in_msg, char* out_error_msg)
{
}

int msg_Parse_Post(char* in_msg, char* out_author, char* out_title, char* out_contents)
{
}

int msg_Parse_Read(char* in_msg, uint32_t* out_page_size, uint32_t* out_page_number)
{
}

int msg_Parse_ReadResponse(char* in_msg, uint32_t* out_article_count, Article* out_articles)
{
}

int msg_Parse_Choose(char* in_msg, uint32_t* out_article_id)
{
}

int msg_Parse_ChooseResponse(char* in_msg, Article* out_articles)
{
}

int msg_Parse_Reply(char* in_msg, uint32_t* out_article_id, char* out_author, char* out_contents)
{
}

int msg_Build_ErrorResponse(char* out_msg, char* in_err_string)
{
}

int msg_Build_Post(char* out_msg, char* in_author, char* in_title, char* in_contents)
{
}

int msg_Build_PostResponse(char* out_msg)
{
}

int msg_Build_Read(char* out_msg, uint32_t page_size, uint32_t page_number)
{
}

int msg_Build_ReadResponse(char* out_msg, int article_count, Article articles[])
{
}

int msg_Build_Choose(char* out_msg, uint32_t article_id)
{
}

int msg_Build_ChooseResponse(char* out_msg, Article articles)
{
}

int msg_Build_Reply(char* out_msg, uint32_t article_id, char* author, char* contents)
{
}

int msg_Build_ReplyResponse(char* out_msg)
{
}
