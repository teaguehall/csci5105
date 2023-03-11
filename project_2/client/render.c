#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../shared/article.h"

// clears terminal
int render_ClearTerminal(void)
{
    if(system("clear") == -1)
    {
        fprintf(stderr, "ERROR: Failed to clear terminal during article rendering: %s\n", strerror(errno));
        return -1;
    }

    // success
    return 0;
}

// formats an article output to the terminal
int render_Article(Article article)
{
    int char_count = 0;

    // clear terminal on start-up
    render_ClearTerminal();

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
int render_List(int page_size, int article_count, Article articles[])
{
    char command[4096];
    int total_pages;
    int curr_page = 1;
    int index_1;
    int index_2;
    int i;
    
    // calculate total number of pages
    total_pages = article_count / page_size + 1;
    
    // keep showing pages until user types exit
    while(1)
    {
        // determine which articles to show (dependent on the page number)
        index_1 = (curr_page - 1) * page_size;
        index_2 = index_1 + page_size;
        if(index_2 > article_count) index_2 = article_count; // truncate last index if it goes beyond the article count
        
        // clear terminal on start-up
        render_ClearTerminal();
        
        // write article header
        printf("----------------------------------------------------------------------------------------------\n");
        printf(" Article List (page %u of %u)\n", curr_page, total_pages);
        printf("----------------------------------------------------------------------------------------------\n\n");
        
        // print article page
        for(i = index_1; i < index_2; i++)
        {
            // print indentation depending on depth
            for(int j = 0; j < articles[i].depth; j++)
            {
                putchar('\t');
            }

            // print article info
            printf(" %u. \"%s\" by %s | %.48s...\n", articles[i].id, articles[i].title, articles[i].author, articles[i].contents);
        }

        // prompt user for next input
        printf("\n----------------------------------------------------------------------------------------------\n");
        printf(" Type: exit (to exit), next (for next page), prev (for prev page)\n");
        printf("----------------------------------------------------------------------------------------------\n");
        
        // read  user input
        if(fgets(command, sizeof(command), stdin) == NULL)
        {
            fprintf(stderr, "ERROR: fgets call failed with error \"%s\"\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        command[strcspn(command, "\n")] = 0; // remove new line character

        // handle user input
        if(strcmp(command, "next") == 0)
        {
            if(curr_page < total_pages) curr_page++;
        }
        else if(strcmp(command, "prev") == 0)
        {
            if(curr_page > 1 ) curr_page--;
        }
        else if(strcmp(command, "exit") == 0)
        {
            break;
        }
    }

    // clear terminal on exit
    render_ClearTerminal();

    // success
    return 0;
}
