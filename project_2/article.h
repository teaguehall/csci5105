#ifndef ARTICLE_H
#define ARTICLE_H

#define ARTICLE_MAX_AUTHOR_NAME 128
#define ARTICLE_MAX_TITLE 128
#define ARTICLE_MAX_CONTENTS 1024

typedef struct Article {
    int id;
    int parent_id;
    char author[ARTICLE_MAX_AUTHOR_NAME];
    char title[ARTICLE_MAX_TITLE];
    char contents[ARTICLE_MAX_CONTENTS];
} Article;

#endif // ARTICLE_H
