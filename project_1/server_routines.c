#include "communicate.h"
#include <stdio.h>
#include <pthread.h>
#include <errno.h>



// shared variables
pthread_mutex_t lock_client_count = PTHREAD_MUTEX_INITIALIZER;
int client_count = 0;
#define MAXCLIENT	4

int *
join_1_svc(char *ip, int port,  struct svc_req *rqstp)
{
	static int result;

	if(pthread_mutex_lock(&lock_client_count) != 0)
	{
		fprintf(stderr, "ERROR: Server JOIN failed to lock mutex: %s. Exiting...", strerror(errno));
        exit(EXIT_FAILURE);
	}

	if(client_count < MAXCLIENT) 
	{
		client_count++;
		printf("JOIN: Client \"%s:%d\" successfully joined the group. Count at %d/%d clients\n", ip, port, client_count, MAXCLIENT);
		result = 0;
	}
	else
	{
		printf("JOIN: Client \"%s:%d\" failed to join group. Count full at %d/%d clients\n", ip, port, client_count, MAXCLIENT);
		result = 1;
	}

	if(pthread_mutex_unlock(&lock_client_count) != 0)
	{
		fprintf(stderr, "ERROR: Server JOIN failed to unlock mutex: %s. Exiting...", strerror(errno));
        exit(EXIT_FAILURE);
	}

	return &result;
}

int *
leave_1_svc(char *ip, int port,  struct svc_req *rqstp)
{
	static int result;

	if(pthread_mutex_lock(&lock_client_count) != 0)
	{
		fprintf(stderr, "ERROR: Server LEAVE failed to lock mutex: %s. Exiting...", strerror(errno));
        exit(EXIT_FAILURE);
	}

	if(client_count > 0) 
	{
		client_count--;
		printf("LEAVE: Client \"%s:%d\" successfully left the group. Count at %d/%d clients\n", ip, port, client_count, MAXCLIENT);
		result = 0;
	}
	else
	{
		printf("LEAVE: Client \"%s:%d\" failed to leave group. Count was already at %d/%d clients\n", ip, port, client_count, MAXCLIENT);
		result = 1;
	}

	if(pthread_mutex_unlock(&lock_client_count) != 0)
	{
		fprintf(stderr, "ERROR: Server LEAVE failed to unlock mutex: %s. Exiting...", strerror(errno));
        exit(EXIT_FAILURE);
	}

	return &result;
}

int *
subscribe_1_svc(char *ip, int port, char *article,  struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
unsubscribe_1_svc(char *ip, int port, char *article,  struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
publish_1_svc(char *ip, int port, char *article,  struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
ping_1_svc(struct svc_req *rqstp)
{
	static int  result;
	return &result;
}
