#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "net.h"
#include "render.h"
#include "../shared/msg.h"
#include "../shared/article.h"
#include "../shared/tcp.h"

#include <stdint.h>
#include <arpa/inet.h>

int main(int argc, char * argv[])
{
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

        exit(EXIT_FAILURE);
    }

    // validate server address
    if(!tcp_IpAddrIsValid(argv[2]))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    // validate port number
    if(!tcp_PortIsValid(atoi(argv[3])))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    
    // save server connect info for later use
    ServerInfo connect_info;
    strcpy(connect_info.address, argv[2]);
    connect_info.port = atoi(argv[3]);

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
        article_title[0] = 0;
        article_contents[0] = 0;
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////// GET USER INPUT ////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        printf("Enter command: POST, READ, CHOOSE, or REPLY\n");
        
        if(fgets(command, sizeof(command), stdin) == NULL)
        {
            fprintf(stderr, "ERROR: fgets call failed with error \"%s\"\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        command[strcspn(command, "\n")] = 0; // remove new line character

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////// POST HANDLER  ///////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
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
            if(strlen(article_title) == 0)
            {
                fprintf(stderr, "ERROR: Empty article title was provided in POST. Expected formatting \"POST;<title>;<contents>\"\n");
                continue;
            }

            // extract article contents
            memcpy(article_contents, command + semicolon_2_pos + 1, strlen(command) - semicolon_2_pos - 1);
            if(strlen(article_contents) == 0)
            {
                fprintf(stderr, "ERROR: Empty article content was provided in POST. Expected formatting \"POST;<title>;<contents>\"\n");
                continue;
            }

            // post article to server
            if(net_Post(connect_info, argv[3], article_title, article_contents))
            {
                fprintf(stderr, "ERROR: Client failed to POST article\n");
            }
            else
            {
                printf("Client successfully posted article\n");
            }

        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////// READ HANDLER  ///////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        else if(strncmp(command, "READ", sizeof("READ")) == 0)
        {
            // TODO
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////// CHOOSE HANDLER  //////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        else if(strncmp(command, "CHOOSE", sizeof("CHOOSE")) == 0)
        {
            // TODO
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////// REPLY HANDLER  ///////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
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