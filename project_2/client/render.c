#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// formats an article output to the terminal
int render_Article(Article article)
{
    int char_count = 0;

    // clear terminal
    if(system("clear") == -1)
    {
        fprintf(stderr, "ERROR: Failed to clear terminal during article rendering: %s\n", strerror(errno));
        return -1;
    }

    // write article header
    printf("---------------------------------------------------------------\n");
    printf("Title: %s\n", article.title);
    printf("Author: %s\n", article.author);
    printf("---------------------------------------------------------------\n\n");

    // write article (chunk string into 64 character lines)
    for(int i = 0; i < strlen(article.contents); i++)
    {
        // create new line every so often
        if(article.contents[i] == ' ' && char_count > 50)
        {
            putchar('\n');
            char_count = 0;
        }
        else
        {
            // write char to terminal
            putchar(article.contents[i]);
        }

        char_count++;
    }

    // write article footer
    printf("\n\n---------------------------------------------------------------\n\n");

    // success
    return 0;
}

// formats a list of article outputs to the terminal
int render_List(int page, int total_pages, int article_count, Article articles[])
{
    // clear terminal
    if(system("clear") == -1)
    {
        fprintf(stderr, "ERROR: Failed to clear terminal during article rendering: %s\n", strerror(errno));
        return -1;
    }

    // write article header
    printf("------------------------------------------------------------------------------------------------------------------------------\n");
    printf(" Article List (page %u of %u)\n", page, total_pages);
    printf("------------------------------------------------------------------------------------------------------------------------------\n\n");

    // print articles
    for(int i = 0; i < article_count; i++)
    {
        // print indentation depending on depth
        for(int j = 0; j < articles[i].depth; j++)
        {
            putchar('\t');
        }
        
        // print article info
        printf(" %u. \"%s\" by %s | %.48s...\n", articles[i].id, articles[i].title, articles[i].author, articles[i].contents);
    }

    printf("\n------------------------------------------------------------------------------------------------------------------------------\n");
    printf(" Navigation: x (to exit), right-arrow (next page), left-arrow (prev page)\n");
    printf("------------------------------------------------------------------------------------------------------------------------------\n");
    
    // success
    return 0;
}