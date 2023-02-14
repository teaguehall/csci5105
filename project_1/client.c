#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "communicate.h"
#include "article.h"

#include <netdb.h>
#include <sys/types.h>

// thread function prototypes
void* sendThreadFun(void* arg);
void* recvThreadFun(void* arg);
void* pingThreadFun(void* arg);

pthread_mutex_t lock_rpc = PTHREAD_MUTEX_INITIALIZER;

// thread varibles
CLIENT *clnt;
char client_hostname[256];
char client_addr[INET_ADDRSTRLEN];
int client_port;
int socket_fd;
struct sockaddr_in cliaddr, servaddr;

// test variables
Article test_article;
int test_article_inputted;
int test_unsubscribe_duration;
int test_article_is_publisher;
void testRoutine(void);

int main(int argc, char* argv[])
{
    // validate input args
    if(argc < 2 || argc > 4)
    {
        fprintf (stderr, "ERROR: Invalid number of input args received. Expected:\n");
        fprintf (stderr, "  #1 (required) - Group Server Name\n");
        fprintf (stderr, "  #2 (optional) - Test Article\n");
        fprintf (stderr, "  #3 (optional) - Test Unsubscribe Time(sec)\n");
		exit(EXIT_FAILURE);
    }

    // save group server name
    char* group_server = argv[1];

    // save and validate test article
    test_article_inputted = 0;
    if(argc > 2)
    {
        test_article_inputted = 1;
        
        if(articleDecode(argv[2], &test_article))
        {
            fprintf(stderr, "ERROR: Test article failed validdation");
            exit(EXIT_FAILURE);
        }        
    }

    // save and validate unsubscribe duration
    test_unsubscribe_duration = 0;
    if(argc > 3)
    {
        test_unsubscribe_duration = atoi(argv[3]);
    }

    // Create RPC client
    printf("INFO: Creating RPC client for remote host: %s...\n", group_server);  
    clnt = clnt_create (group_server, COMMUNICATE_PROG, COMMUNICATE_VERSION, "udp");
    if (clnt == NULL) 
    {
        clnt_pcreateerror (group_server);
        exit (EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliaddr.sin_port = 0; // listen on any available port

    // create listening socket
    if((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        fprintf(stderr, "ERROR: Client failed to create UDP listening socket: %s. Exiting...", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // bind listening socket to available address/port
    if(bind(socket_fd, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) == -1)
    {
        fprintf(stderr, "ERROR: Failed to bind socket to port: %s. Exiting...", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // determine IP address to send to server
    // TODO - is it okay to send name instead of IP address? 
    gethostname(client_hostname, sizeof(client_hostname));

    // get port that socket is attached to
    struct sockaddr_in temp;
    socklen_t len = sizeof(temp);

    if (getsockname(socket_fd, (struct sockaddr *)&temp, &len) == -1)
    {
        fprintf(stderr, "ERROR: Failed to identify listening port: %s. Exiting...", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        client_port = ntohs(temp.sin_port);
    }

    // send RPC-join message
    int* ptr_result;
    if((ptr_result = join_1(client_hostname, client_port, clnt)) == NULL)
    {
        clnt_perror(clnt, "RPC-JOIN failed. Exiting...");
        exit(EXIT_FAILURE);
    }
    else if(*ptr_result)
    {
        fprintf(stderr, "Client failed to join group server. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    
    // spawn threads
    pthread_t send_thread_id;
    pthread_t recv_thread_id;
    pthread_t ping_thread_id;

    if(pthread_create(&ping_thread_id, NULL, pingThreadFun, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to spawn thread pingThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&recv_thread_id, NULL, recvThreadFun, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to spawn thread recvThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&send_thread_id, NULL, sendThreadFun, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to spawn thread sendThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // join threads
    if(pthread_join(send_thread_id, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to join sendThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(pthread_join(recv_thread_id, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to join recvThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(pthread_join(ping_thread_id, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to join pingThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }   

    // done
    return 0; 
}

// thread reponsible for reading user input and sending requests
void* sendThreadFun(void* arg)
{
    Article article;
    char input[512];
    int* ptr_result;

    // if test parameters provided, execute them:
    if(test_article_inputted)
    {
        // decode test article into object
        if(articleDecode(input, &article))
        {
            fprintf(stderr, "ERROR: Provided test article failed validation. Exiting...\n");
            exit(EXIT_FAILURE);;
        }

        // determine article type
        if(article.contents[0] == '\0')
        {
            test_article_is_publisher  = 0; // subscriber article
        }
        else
        {
            test_article_is_publisher  = 1; // publisher article
        }

        // grab rpc lock 
        if(pthread_mutex_lock(&lock_rpc) != 0)
        {
            fprintf(stderr, "ERROR: failed to lock rpc mutex: %s. Exiting...", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // send message depending on type
        if(test_article_is_publisher)
        {
            if((ptr_result = publish_1(client_hostname, client_port, input, clnt)) == NULL)
            {
                fprintf(stderr, "ERROR: Server failed to respond to PUBLISH message. Exiting...\n");
                exit(EXIT_FAILURE);
            }
            else if(*ptr_result)
            {
                fprintf(stderr, "Client publish request failed. Exiting...\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            if((ptr_result = subscribe_1(client_hostname, client_port, input, clnt)) == NULL)
            {
                fprintf(stderr, "ERROR: Server failed to respond to SUBSCRIBE message. Exiting...\n");
                exit(EXIT_FAILURE);
            }
            else if(*ptr_result)
            {
                fprintf(stderr, "Client subscribe request failed. Exiting...\n");
                exit(EXIT_FAILURE);
            }
        }

        // release RPC lock
        if(pthread_mutex_unlock(&lock_rpc) != 0)
        {
            fprintf(stderr, "ERROR: failed to unlock rpc mutex: %s. Exiting...", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // TODO check for unsubscribe duration

    
        printf( "INFO: Client finished testing. Exiting...\n");
        exit(EXIT_SUCCESS);
    }



    printf("INFO: Starting user-input thread...\n");
    
    // read user inputs indefinitely 
    while(1)
    {
        printf("enter command: leave, subscribe, unsubscribe, publish\n");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // remove new line character

        //////////////////////////////////////// leave handler //////////////////////////////////////////
        if(strcmp("leave", input) == 0)
        {
            // grab rpc lock
            if(pthread_mutex_lock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to lock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // send leave message
            if((ptr_result = leave_1(client_hostname, client_port, clnt)) == NULL)
            {
                fprintf(stderr, "ERROR: Server failed to respond to LEAVE. Exiting...\n");
                exit(EXIT_FAILURE);
            }

            // release rpc lock
            if(pthread_mutex_unlock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to unlock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // terminate program since client has left group server
            printf("Client has left group server. Good bye\n");
            exit(EXIT_SUCCESS);
        }
        ///////////////////////////////////// subscribe handler ////////////////////////////////////////
        else if(strcmp("subscribe", input) == 0)
        {
            printf("enter subscribe article: <type>;<originator>;<org>;\n");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0; // remove new line character

            // validate article
            if(articleDecode(input, &article))
            {
                fprintf(stderr, "ERROR: Article format failed validation. Try again...\n");
                continue;
            }

            // make sure message does not contain contents
            if(article.contents[0] != '\0')
            {
                fprintf(stderr, "ERROR: Subscribe article has non-null contents:%s. Try again...\n", article.contents);
                continue;
            }
            
            // grab rpc lock
            if(pthread_mutex_lock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to lock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // send subscribe message
            if((ptr_result = subscribe_1(client_hostname, client_port, input, clnt)) == NULL)
            {
                fprintf(stderr, "ERROR: Server failed to respond to SUBSCRIBE message. Exiting...\n");
                exit(EXIT_FAILURE);
            }
            else if(*ptr_result)
            {
                fprintf(stderr, "Client subscribe request failed. Try again...\n");
            }

            // release rpc lock
            if(pthread_mutex_unlock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to unlock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        //////////////////////////////////// unsubscribe handler ///////////////////////////////////////
        else if(strcmp("unsubscribe", input) == 0)
        {
            printf("enter unsubscribe article: <type>;<originator>;<org>;\n");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0; // remove new line character

            // validate article
            if(articleDecode(input, &article))
            {
                fprintf(stderr, "ERROR: Article format failed validation. Try again...\n");
                continue;
            }

            // make sure message does not contain contents
            if(article.contents[0] != '\0')
            {
                fprintf(stderr, "ERROR: Subscribe article has non-null contents:%s. Try again...\n", article.contents);
                continue;
            }
            
            // grab rpc lock
            if(pthread_mutex_lock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to lock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // send unsubscribe message
            if((ptr_result = unsubscribe_1(client_hostname, client_port, input, clnt)) == NULL)
            {
                fprintf(stderr, "ERROR: Server failed to respond to UNSUBSCRIBE message. Exiting...\n");
                exit(EXIT_FAILURE);
            }
            else if(*ptr_result)
            {
                fprintf(stderr, "Client unsubscribe request failed. Try again...\n");
            }

            // release rpc lock
            if(pthread_mutex_unlock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to unlock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        ///////////////////////////////////// publish handler ////////////////////////////////////////
        else if(strcmp("publish", input) == 0)
        {
            printf("enter publish article: <type>;<originator>;<org>;<contents>\n");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0; // remove new line character

            // validate article
            if(articleDecode(input, &article))
            {
                fprintf(stderr, "ERROR: Article format failed validation. Try again...\n");
                continue;
            }

            // make sure message contains content
            if(article.contents[0] == '\0')
            {
                fprintf(stderr, "ERROR: Published article has no contents:%s. Try again...\n", article.contents);
                continue;
            }
            
            // grab rpc lock
            if(pthread_mutex_lock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to lock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }

            // send unsubscribe message
            if((ptr_result = publish_1(client_hostname, client_port, input, clnt)) == NULL)
            {
                fprintf(stderr, "ERROR: Server failed to respond to UNSUBSCRIBE message. Exiting...\n");
                exit(EXIT_FAILURE);
            }
            else if(*ptr_result)
            {
                fprintf(stderr, "Client unsubscribe request failed. Try again...\n");
            }

            // release rpc lock
            if(pthread_mutex_unlock(&lock_rpc) != 0)
            {
                fprintf(stderr, "ERROR: failed to unlock rpc mutex: %s. Exiting...", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Unrecognized command. Try again...\n");
        }
    }
}

// thread for recving articles
void* recvThreadFun(void* arg)
{
    socklen_t len;
    static char rcvd_article[sizeof(Article)];

    len = sizeof(cliaddr);  //len is value/result

    printf("INFO: Starting message receive thread...\n");

    while(recvfrom(socket_fd, rcvd_article, sizeof(Article), MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len) !=-1)
    {
        // ensure null termination
        rcvd_article[sizeof(Article) - 1] = '\0';

        // print received article
        printf("Received article: %s\n", rcvd_article);
    }

    // will only get here if error occurs
    fprintf(stderr, "ERROR: Failed receiving server message from socket: %s. Exiting...", strerror(errno));
}

// thread responsible for pringing RPC
void* pingThreadFun(void* arg)
{
    int* ptr_result;

    printf("INFO: Starting ping thread...\n");

    // ping indefinitely
    while(1)
    {
        // grab rpc lock
        if(pthread_mutex_lock(&lock_rpc) != 0)
        {
            fprintf(stderr, "ERROR: Ping thread failed to lock mutex: %s. Exiting...", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // send ping message
        if((ptr_result = ping_1(client_hostname, client_port, clnt)) == NULL)
        {
            fprintf(stderr, "ERROR: Server failed to respond to PING. Exiting...\n");
            exit(EXIT_FAILURE);
        }

        // release rpc lock
        if(pthread_mutex_unlock(&lock_rpc) != 0)
        {
            fprintf(stderr, "ERROR: Ping thread failed to unlock mutex: %s. Exiting...", strerror(errno));
            exit(EXIT_FAILURE);
        }

        // sleep for a bit
        sleep(1);
    }
}
