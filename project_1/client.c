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
    Article test_article;
    int test_article_inputted = 0;
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
    int test_unsubscribe_duration = 0;
    if(argc > 3)
    {
        test_unsubscribe_duration = atoi(argv[3]);
    }

    // RPC connect to remote host
    printf("Connecting to remote host: %s\n", group_server);  
    clnt = clnt_create (group_server, COMMUNICATE_PROG, COMMUNICATE_VERSION, "udp");
    if (clnt == NULL) 
    {
        clnt_pcreateerror (group_server);
        exit (EXIT_FAILURE);
    }
    printf("Successfully connected to remote host: %s\n", group_server);  

    int socket_fd;
    struct sockaddr_in cliaddr, servaddr;

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

    //char* my_addr = inet_ntoa(cliaddr.sin_addr);
    //printf("my_addr = %s\n", my_addr);
    //printf("client_addr = %s\n", client_addr);
    //printf("client_port = %d\n", client_port);

    // send RCP-join message
    int* ptr_result;
    if((ptr_result = join_1(client_hostname, client_port, clnt)) == NULL)
    {
        clnt_perror(clnt, "call failed");   
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

    if(pthread_create(&send_thread_id, NULL, sendThreadFun, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to spawn thread sendThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&recv_thread_id, NULL, recvThreadFun, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to spawn thread recvThreadFun: %s. Exiting...\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&ping_thread_id, NULL, pingThreadFun, NULL) == -1)
    {
        fprintf(stderr, "ERROR: Failed to spawn thread pingThreadFun: %s. Exiting...\n", strerror(errno));
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

    // leave server
    if((ptr_result = leave_1(client_hostname, client_port, clnt)) == NULL)
    {
        clnt_perror(clnt, "call failed");   
    }
    else if(*ptr_result)
    {
        fprintf(stderr, "Client failed to join group server. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    // destroy RPC client
	clnt_destroy(clnt);
    return 0; 
}

// thread reponsible for reading user input and sending requests
void* sendThreadFun(void* arg)
{
    printf("hello from thread 1\n");
}

// thread for recving articles
void* recvThreadFun(void* arg)
{
    //// test
    //printf("port number = %d\n", ntohs(cliaddr.sin_port));
//
//
    //// grab host name and available port for UDP listening
//
    //// test
    //char buffer[256];
    //gethostname(buffer, sizeof(buffer));
    //printf("hostname = %s\n", buffer);
//
    //// register to group server
    //int* result = join_1("client #1", 64, clnt);
	//if (result == (int *) NULL) {
	//	clnt_perror (clnt, "call failed");
	//}

    sleep(5);














}

// thread responsible for pringing RPC
void* pingThreadFun(void* arg)
{
    int* ptr_result;

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
        if((ptr_result = ping_1(clnt)) == NULL)
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
