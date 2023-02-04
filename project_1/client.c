#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "communicate.h"
#include "article.h"

// thread function prototypes
void* sendThreadFun(void* arg);
void* recvThreadFun(void* arg);
void* pingThreadFun(void* arg);

int main(int argc, char* argv[])
{
    CLIENT *clnt; // rpc connection client
    int listener_fd; // udp listening socket for incoming articles

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
    
    // create threads
    pthread_t send_thread_id;
    pthread_t recv_thread_id;
    pthread_t ping_thread_id;

    pthread_create(&send_thread_id, NULL, sendThreadFun, NULL);
    pthread_create(&recv_thread_id, NULL, recvThreadFun, NULL);
    pthread_create(&ping_thread_id, NULL, pingThreadFun, NULL);
    

    // RPC connect to remote host
    printf("Connecting to remote host: %s\n", argv[1]);  
    clnt = clnt_create (argv[1], COMMUNICATE_PROG, COMMUNICATE_VERSION, "udp");
    if (clnt == NULL) 
    {
        clnt_pcreateerror (argv[1]);
        exit (-1);
    }

    // create and bind UDP listening socket
    struct sockaddr_in cliaddr, servaddr;

    memset(&cliaddr, 0, sizeof(cliaddr));
    memset(&servaddr, 0, sizeof(servaddr));

    if((listener_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        fprintf(stderr, "ERROR: Client failed to create UDP listening socket: %s. Exiting...", strerror(errno));
        exit(-1);
    }

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliaddr.sin_port = 0; // listen on any available port

    if(bind(listener_fd, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) == -1)
    {
        fprintf(stderr, "ERROR: Failed to bind socket to port: %s. Exiting...", strerror(errno));
        exit(-1);
    }

    // grab listening host name + port number
    struct sockaddr temp;
    int namelen;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(listener_fd, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
        printf("port number %d\n", ntohs(sin.sin_port));


    // grab host name and available port for UDP listening

    // test
    char buffer[256];
    gethostname(buffer, sizeof(buffer));
    printf("hostname = %s\n", buffer);

    // register to group server
    int* result = join_1("client #1", 64, clnt);
	if (result == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}

    sleep(5);

	//result_1 = join_1(join_1_arg1, join_1_arg2, clnt);
	//if (result_1 == (int *) NULL) {
	//	clnt_perror (clnt, "call failed");
	//}
	//result_2 = leave_1(leave_1_arg1, leave_1_arg2, clnt);
	//if (result_2 == (int *) NULL) {
	//	clnt_perror (clnt, "call failed");
	//}
	//result_3 = subscribe_1(subscribe_1_arg1, subscribe_1_arg2, subscribe_1_arg3, clnt);
	//if (result_3 == (int *) NULL) {
	//	clnt_perror (clnt, "call failed");
	//}
	//result_4 = unsubscribe_1(unsubscribe_1_arg1, unsubscribe_1_arg2, unsubscribe_1_arg3, clnt);
	//if (result_4 == (int *) NULL) {
	//	clnt_perror (clnt, "call failed");
	//}
	//result_5 = publish_1(publish_1_arg1, publish_1_arg2, publish_1_arg3, clnt);
	//if (result_5 == (int *) NULL) {
	//	clnt_perror (clnt, "call failed");
	//}
	//result_6 = ping_1(clnt);
	//if (result_6 == (int *) NULL) {
	//	clnt_perror (clnt, "call failed");
	//}

    // leave TODO
	clnt_destroy (clnt);
    return 0; 
}

// thread reponsible for reading user input and sending requests
void* sendThreadFun(void* arg)
{

}

// thread for recving articles
void* recvThreadFun(void* arg)
{

}

// thread responsible for pringing RPC
void* pingThreadFun(void* arg)
{
    
}