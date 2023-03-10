/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef ARTICLE_H
#define ARTICLE_H

#define ARTICLE_MAX_BYTES 120



typedef struct Article {
    
    char type[ARTICLE_MAX_BYTES];
    char originator[ARTICLE_MAX_BYTES];
    char org[ARTICLE_MAX_BYTES];
    char contents[ARTICLE_MAX_BYTES];
} Article;

int articleDecode(char* bytes, Article* output);

#endif // ARTICLE_H
