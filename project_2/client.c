#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "article.h"

int main(int argc, char * argv[])
{
    char command[1024];
    int semicolon_count;
    int semicolon_1_pos;
    int semicolon_2_pos;
    int i; 

    int article_id;
    int article_title[ARTICLE_MAX_TITLE];
    int article_contents[ARTICLE_MAX_CONTENTS];

    // verify input arguments
    if(argc < 3 || argc > 3)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "EROR: Unexpected number of input arguments received. Expected:\n");
        fprintf(stderr, " * REQUIRED: Your Name (posts will be tagged with your name as the author)\n");
        fprintf(stderr, " * REQUIRED: Server Hostname (name of the BB server to connect to)\n");
        // TODO add optional args for test scenarios
        fprintf(stderr, "\n");

        return EXIT_FAILURE;
    }

    // connect to server
    // TODO

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
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // remove new line character

        // handle user input
        if(strncmp(command, "POST", sizeof("POST")) == 0)
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

            // POST article to server
            // TODO

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