#ifndef CLIENT_RENDER_H
#define CLIENT_RENDER_H

#include "../shared/article.h"

int render_Article(Article article);
int render_List(int page_size, int article_count, Article articles[]);
int render_ClearTerminal(void);

#endif // CLIENT_RENDER_H
