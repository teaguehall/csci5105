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
int render_List(Article articles[])
{
    // success
    return 0;
}
