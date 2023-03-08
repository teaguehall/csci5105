#ifndef CLIENT_RENDER_H
#define CLIENT_RENDER_H

#include "../shared/article.h"

int render_Article(Article article);
int render_List(int page, int total_pages, int article_count, Article articles[]);

#endif // CLIENT_RENDER_H
