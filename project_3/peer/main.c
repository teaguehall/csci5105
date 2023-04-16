#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "../shared/peer_info.h"
#include "../shared/tcp.h"

//#include "render.h"
//#include "../shared/net.h"
//#include "../shared/msg.h"
//#include "../shared/article.h"


//
//#include <stdint.h>
//#include <arpa/inet.h>
//
//#define PAGE_SIZE           10
//
//static Article article_buffer[MAX_ARTICLES];


int main(int argc, char* argv[])
{
    PeerInfo peer;
    
    
    // verify number of input arguments
    if(argc < 7 || argc > 7)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, "ERROR: Unexpected number of input arguments received. Expected:\n");
        fprintf(stderr, " * REQUIRED: Peer - Binding Interface (e.g. 127.0.0.1)\n");
        fprintf(stderr, " * REQUIRED: Peer - Path to Shared Directory\n");
        fprintf(stderr, " * REQUIRED: Peer - Latitude Location (-90 to 90) \n");
        fprintf(stderr, " * REQUIRED: Peer - Latitude Location (-180 to 180) \n");
        fprintf(stderr, " * REQUIRED: Server - Address (xxx.xxx.xxx.xxx)\n");
        fprintf(stderr, " * REQUIRED: Server - Port\n");
        // TODO add optional args for test scenarios
        fprintf(stderr, "\n");
        exit(EXIT_FAILURE);
    }

    // validate binding address for listener
    if(!tcp_IpAddrIsValid(argv[1]))
    {
        fprintf(stderr, "ERROR: Invalid binding interface for listening \"%s\" specified\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    else
    {
        memcpy(peer.address, argv[1], strlen(argv[1]) + 1);
    }

    // verify shared folder exists
     DIR *shared_folder;
    shared_folder = opendir(argv[2]);
    if(shared_folder == NULL)
    {
        fprintf(stderr, "Error opening shared folder \"%s\": %s\n", argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }
    closedir(shared_folder);

    char* endptr;

    // validate latitude coordinate
    peer.latitude = strtod(argv[3], &endptr);
    if(endptr == argv[3] || peer.latitude < -90.0 || peer.latitude > 90.0)
    {
        fprintf(stderr, "ERROR: Invalid latitude of node \"%s\" provided (-90.0 thru 90.0 allowed)\n", argv[3]);
        exit(EXIT_FAILURE);
    }

    // validate longitude coordinate
    peer.longitude = strtod(argv[4], &endptr);
    if(endptr == argv[4] || peer.longitude < -180.0 || peer.longitude > 180.0)
    {
        fprintf(stderr, "ERROR: Invalid longitude of node \"%s\" provided (-180.0 thru 180.0 allowed)\n", argv[4]);
        exit(EXIT_FAILURE);
    }

    // validate server address
    strcpy(peer.address, argv[5]);
    if(!tcp_IpAddrIsValid(peer.address))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided\n", argv[5]);
        exit(EXIT_FAILURE);
    }

    // validate server port
    peer.port = atoi(argv[6]);
    if(!tcp_PortIsValid(peer.port))
    {
        fprintf(stderr, "ERROR: Invalid server address \"%s\" provided\n", argv[6]);
        exit(EXIT_FAILURE);
    }

    // start ping thread
    // response for publishing 

    // start UI thread

    // start ping thread

    // update 

    // start ping thread

    // start connection handler thread
    // TODO

    // start 

    
    
    //// save server info 
    //char* server_addr;
    //int server_port;
    //server_addr = argv[2];
    //server_port = atoi(argv[3]);
//
    //// clear terminal
    //render_ClearTerminal();
//
    //// display message formatting instructions
    //printf("--------------------------------------------\n");
    //printf("Supported messages + formatting instructions\n");
    //printf("--------------------------------------------\n");
    //printf("POST;<title>;<contents>\n");
    //printf("READ\n");
    //printf("CHOOSE;<article id>\n");
    //printf("REPLY;<article id>;<response>\n");
    //printf("--------------------------------------------\n\n");
//
    //// command loop
    //while(1)
    //{
    //    semicolon_count = 0;
//
    //    memset(article_title, 0, sizeof(article_title));
    //    memset(article_contents, 0, sizeof(article_contents));
    //    memset(article_id_str, 0, sizeof(article_id_str));
    //    
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    ///////////////////////////////////////// GET USER INPUT ////////////////////////////////////////////
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    printf("Enter command: POST, READ, CHOOSE, REPLY, or HELP\n");
    //            
    //    if(fgets(command, sizeof(command), stdin) == NULL)
    //    {
    //        fprintf(stderr, "ERROR: fgets call failed with error \"%s\"\n", strerror(errno));
    //        exit(EXIT_FAILURE);
    //    }
//
    //    command[strcspn(command, "\n")] = 0; // remove new line character
//
    //    render_ClearTerminal(); // always clear terminal to clean up before next output message
//
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    /////////////////////////////////////////// POST HANDLER  ///////////////////////////////////////////
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    if(strncmp(command, "POST", 4) == 0 || strncmp(command, "post", 4) == 0)
    //    {
    //        // find semi-colon locations in "POST;<title>;<contents>"
    //        for(i = 0; i < strlen(command); i++)
    //        {
    //            if(command[i] == ';')
    //            {
    //                semicolon_count++;
//
    //                if(semicolon_count == 1)
    //                    semicolon_1_pos = i;
    //                else if(semicolon_count == 2)
    //                    semicolon_2_pos = i;
    //            }
    //        }
    //        if(semicolon_count != 2)
    //        {
    //            fprintf(stderr, "ERROR: Invalid number of semi-colons found in POST message. Expected formatting \"POST;<title>;<contents>\"\n");
    //            continue;
    //        }
//
    //        // extract article title
    //        memcpy(article_title, command + semicolon_1_pos + 1, semicolon_2_pos - semicolon_1_pos - 1);
    //        if(strlen(article_title) == 0)
    //        {
    //            fprintf(stderr, "ERROR: Empty article title was provided in POST. Expected formatting \"POST;<title>;<contents>\"\n");
    //            continue;
    //        }
//
    //        // extract article contents
    //        memcpy(article_contents, command + semicolon_2_pos + 1, strlen(command) - semicolon_2_pos - 1);
    //        if(strlen(article_contents) == 0)
    //        {
    //            fprintf(stderr, "ERROR: Empty article content was provided in POST. Expected formatting \"POST;<title>;<contents>\"\n");
    //            continue;
    //        }
//
    //        // post article to server
    //        if(net_Post(server_addr, server_port, argv[1], article_title, article_contents))
    //        {
    //            fprintf(stderr, "ERROR: Client failed to POST article\n");
    //        }
    //        else
    //        {
    //            printf("Client successfully posted article\n");
    //        }
//
    //    }
//
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    /////////////////////////////////////////// READ HANDLER  ///////////////////////////////////////////
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    else if(strncmp(command, "READ", 4) == 0 || strncmp(command, "read", 4) == 0)
    //    {           
    //        // send read request to article (we just request all articles from server that will fit into our buffer)
    //        if(net_Read(server_addr, server_port, MAX_ARTICLES, &articles_read,  article_buffer))
    //        {
    //            fprintf(stderr, "ERROR: Client failed to READ articles\n");
    //            continue;
    //        }
//
    //        // display contents
    //        if(articles_read > 0)
    //        {
    //            // display articles
    //            if(render_List(PAGE_SIZE, articles_read, article_buffer))
    //            {
    //                fprintf(stderr, "ERROR: Error occurred while displaying arcticles\n");
    //            }
//
    //        }
    //        else
    //        {
    //            fprintf(stderr, "WARN: Server returned zero articles\n");
    //        }
    //    }
//
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    ////////////////////////////////////////// CHOOSE HANDLER  //////////////////////////////////////////
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    else if(strncmp(command, "CHOOSE", 6) == 0 || strncmp(command, "choose", 6) == 0)
    //    {
    //        // grab article id from inputted command
    //        article_id = atoi(command + 7); // we add offset to ignore the CHOOSE; field.
    //        if(article_id == 0  && *(command + 7) != '0')
    //        {
    //            fprintf(stderr, "ERROR: Invalid article ID. Expected formatting \"CHOOSE;<article id>\"\n");
    //            continue;
    //        }
    //        
    //        // send choose request to server
    //        if(net_Choose(server_addr, server_port, article_id, article_buffer))
    //        {
    //            fprintf(stderr, "ERROR: Client failed to retrieve article from server\n");
    //            continue;
    //        }
//
    //        // display article
    //        if(render_Article(article_buffer[0]))
    //        {
    //            fprintf(stderr, "ERROR: Failed to render article\n");
    //            continue;
    //        }
    //    }
//
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    ////////////////////////////////////////// REPLY HANDLER  ///////////////////////////////////////////
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    else if(strncmp(command, "REPLY", 5) == 0 || strncmp(command, "reply", 5) == 0)
    //    {
    //        // find semi-colon locations in "REPLY;<article id>;<response>"
    //        for(i = 0; i < strlen(command); i++)
    //        {
    //            if(command[i] == ';')
    //            {
    //                semicolon_count++;
//
    //                if(semicolon_count == 1)
    //                    semicolon_1_pos = i;
    //                else if(semicolon_count == 2)
    //                    semicolon_2_pos = i;
    //            }
    //        }
    //        if(semicolon_count != 2)
    //        {
    //            fprintf(stderr, "ERROR: Invalid number of semi-colons found in REPLY message. Expected formatting \"REPLY;<article id>;<response>\"\n");
    //            continue;
    //        }
//
    //        // extract article ID
    //        memcpy(article_id_str, command + semicolon_1_pos + 1, semicolon_2_pos - semicolon_1_pos - 1);
    //        if(strlen(article_id_str) == 0)
    //        {
    //            fprintf(stderr, "ERROR: No article ID was found in REPLY. Expected formatting \"REPLY;<article id>;<response>\"\n");
    //            continue;
    //        }
//
    //        article_id = atoi(article_id_str);
    //        if(article_id == 0 && *(command + 6) != '0')
    //        {
    //            fprintf(stderr, "ERROR: Invalid article ID \"%s\" found REPLY. Expected formatting \"REPLY;<article id>;<response>\"\n", article_id_str);
    //            continue;
    //        }
//
    //        // extract reply contents
    //        memcpy(article_contents, command + semicolon_2_pos + 1, strlen(command) - semicolon_2_pos - 1);
    //        if(strlen(article_contents) == 0)
    //        {
    //            fprintf(stderr, "ERROR: Empty response was provided in REPLY. Expected formatting \"REPLY;<article id>;<response>\"\n");
    //            continue;
    //        }
//
    //        // send reply to server
    //        if(net_Reply(server_addr, server_port, article_id, argv[1], article_contents))
    //        {
    //            fprintf(stderr, "ERROR: Client failed to submit REPLY to server\n");
    //        }
    //        else
    //        {
    //            printf("Client successfully REPLIED to article\n");
    //        }
    //    }
//
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    ////////////////////////////////////////// HELP HANDLER  ////////////////////////////////////////////
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    else if(strncmp(command, "HELP", sizeof("HELP")) == 0 || strncmp(command, "help", sizeof("help")) == 0)
    //    {
    //        // display message formatting instructions
    //        printf("--------------------------------------------\n");
    //        printf("Supported messages + formatting instructions\n");
    //        printf("--------------------------------------------\n");
    //        printf("POST;<title>;<contents>\n");
    //        printf("READ\n");
    //        printf("CHOOSE;<article id>\n");
    //        printf("REPLY;<article id>;<response>\n");
    //        printf("--------------------------------------------\n\n");
    //    }
//
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    ////////////////////////////////// UNEXPECTED MESSAGE HANDLER  //////////////////////////////////////
    //    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //    else
    //    {
    //        fprintf(stderr, "WARN: Unexpected command \"%s\" received. Try again...\n", command);
    //    }
    //}
//
    //return EXIT_SUCCESS;
}