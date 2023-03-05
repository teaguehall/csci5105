#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "net.h"
#include "render.h"
#include "../shared/msg.h"
#include "../shared/article.h"

#include <stdint.h>
#include <arpa/inet.h>

int main(int argc, char * argv[])
{
    Article article;

    strcpy(article.title, "Help Wanted!");
    strcpy(article.author, "Benjamin J.");
    strcpy(article.contents, "Hello there, my name is Benjamin. This is an interesting way to type articles. I arbitrarily chose line count of 50 for new lines to occurs. However, who knows if it will be sufficient.");
    article.id = 77;
    article.parent_id = 0;
    
    
    //  DEBUG TODO REMOVE AFTER TESTING  
    render_Article(article);
    return 1;


    ServerInfo connect_info;
    
    char command[1024];
    int semicolon_count;
    int semicolon_1_pos;
    int semicolon_2_pos;
    int i; 

    //int article_id;
    char article_title[ARTICLE_MAX_TITLE];
    char article_contents[ARTICLE_MAX_CONTENTS];

    // verify number of input arguments
    if(argc < 4 || argc > 4)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "EROR: Unexpected number of input arguments received. Expected:\n");
        fprintf(stderr, " * REQUIRED: Your Name (posts will be tagged with your name as the author)\n");
        fprintf(stderr, " * REQUIRED: Server Address\n");
        fprintf(stderr, " * REQUIRED: Server Port Number\n");
        // TODO add optional args for test scenarios
        fprintf(stderr, "\n");

        return EXIT_FAILURE;
    }

    // validate connection info
    strcpy(connect_info.address, "127.0.0.1"); // TODO read in arg
    connect_info.port = 5555; // TODO read in arg


    // display message formatting instructions
     printf("\n--------------------------------------------\n");
    printf("Supported messages + formatting instructions\n");
    printf("--------------------------------------------\n");
    printf("POST;<title>;<contents>\n");
    printf("READ\n");
    printf("CHOOSE;<article id>\n");
    printf("REPLY;<article id>\n");
    printf("--------------------------------------------\n\n");

    // command loop
    while(1)
    {
        semicolon_count = 0;
        
        // read user input
        printf("Enter command: POST, READ, CHOOSE, or REPLY\n");
        
        
        if(fgets(command, sizeof(command), stdin) == NULL)
        {
            fprintf(stderr, "ERROR: fgets call failed with error \"%s\"\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        command[strcspn(command, "\n")] = 0; // remove new line character

        // handle user input
        if(strncmp(command, "POST", 4) == 0)
        {
            // find semi-colon locations in "POST;<title>;<contents>"
            for(i = 0; i < strlen(command); i++)
            {
                if(command[i] == ';')
                {
                    semicolon_count++;

                    if(semicolon_count == 1)
                        semicolon_1_pos = i;
                    else if(semicolon_count == 2)
                        semicolon_2_pos = i;
                }
            }
            if(semicolon_count != 2)
            {
                fprintf(stderr, "ERROR: Invalid number of semi-colons found in POST message. Expected formatting \"POST;<title>;<contents>\"\n");
                continue;
            }

            // extract article title
            memcpy(article_title, command + semicolon_1_pos + 1, semicolon_2_pos - semicolon_1_pos - 1);

            // extract article contents
            memcpy(article_contents, command + semicolon_2_pos + 1, strlen(command) - semicolon_2_pos - 1);

            // post article to server
            if(net_Post(connect_info, argv[3], article_title, article_contents))
            {
                fprintf(stderr, "ERROR: Client failed POST article\n");
            }
            else
            {
                printf("Client successfully posted article\n");
            }

        }
        else if(strncmp(command, "READ", sizeof("READ")) == 0)
        {
            // TODO
        }
        else if(strncmp(command, "CHOOSE", sizeof("CHOOSE")) == 0)
        {
            // TODO
        }
        else if(strncmp(command, "REPLY", sizeof("REPLY")) == 0)
        {
            // TODO
        }
        else
        {
            fprintf(stderr, "WARN: Unexpected command \"%s\" received. Try again...\n", command);
        }
    }

    return EXIT_SUCCESS;
}