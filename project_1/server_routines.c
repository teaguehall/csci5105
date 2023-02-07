#include "communicate.h"
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

// active client stuff
#define MAXCLIENTS	4
pthread_mutex_t lock_client_list = PTHREAD_MUTEX_INITIALIZER;
int client_count = 0;
typedef struct ClientEntry
{
	char client_name[120];
	int client_port;
	time_t  last_checkin_sec;
} ClientEntry;
ClientEntry client_list[MAXCLIENTS];

int activeClientAdd(ClientEntry entry);
int activeClientRemove(ClientEntry entry);


// timeout monitoring stuff
#define TIMEOUT_DURATION_SEC		5
pthread_mutex_t lock_timeout = PTHREAD_MUTEX_INITIALIZER;
pthread_t timeout_thread_id;
void* timeoutThreadFun(void* arg);



// subscription list


// shared variables


int *
join_1_svc(char *ip, int port,  struct svc_req *rqstp)
{
	static int result;
	static int timeout_thread_running = 0;
	ClientEntry entry;
	
	// timeout monitoring thread will start when first client joins
	if(!timeout_thread_running)
	{
		if(pthread_create(&timeout_thread_id, NULL, timeoutThreadFun, NULL) == -1)
		{
			fprintf(stderr, "ERROR: Failed to start timeout monitoring thread: %s. Exiting...\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		timeout_thread_running = 1; 
	}

	// copy client name and port number
	strncpy(entry.client_name, ip, sizeof(entry.client_name));
	entry.client_name[sizeof(entry.client_name) - 1] = '\0'; // force null terminate

	entry.client_port = port;

	// add client to active list
	result = activeClientAdd(entry);
	
	return &result;
}

int *
leave_1_svc(char *ip, int port,  struct svc_req *rqstp)
{
	ClientEntry entry;
	static int result;

	// copy client name and port number
	strncpy(entry.client_name, ip, sizeof(entry.client_name));
	entry.client_name[sizeof(entry.client_name) - 1] = '\0'; // force null terminate

	entry.client_port = port;

	// add client to active list
	result = activeClientRemove(entry);

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

// adds client to active list, return 0 on success, -1 on error
int activeClientAdd(ClientEntry entry)
{
	int error = 0; // assume success

	// update last check-in time to current ime
	time(&(entry.last_checkin_sec));
	
	// grab lock for client list
	if(pthread_mutex_lock(&lock_client_list) != 0)
	{
		fprintf(stderr, "ERROR: Failed to grab mutex lock in activeClientAdd: %s", strerror(errno));
        return -1;
	}

	// add client to active list if room available
	if(client_count < MAXCLIENTS)
	{
		// copy client into next available slot
		memcpy(client_list + client_count, &entry, sizeof(entry));

		// increment 
		client_count++;
		printf("Client \"%s:%d\" joined server. Server capacity at %d/%d clients\n", entry.client_name, entry.client_port, client_count, MAXCLIENTS);
	}
	else
	{
		fprintf(stderr, "ERROR: Client attempt join server that is already full at %d/%d clients\n", MAXCLIENTS, MAXCLIENTS);
		error = -1;
	}

	// release lock for client list
	if(pthread_mutex_unlock(&lock_client_list) != 0)
	{
		fprintf(stderr, "ERROR: Failed to release mutex lock in activeClientAdd: %s", strerror(errno));
        return -1;
	}

	return error;
}

// removes client from active list, return 0 on success, -1 on error
int activeClientRemove(ClientEntry entry)
{
	int error = 0; // assume success
	int matched_pos = -1;
	
	// grab lock for client list
	if(pthread_mutex_lock(&lock_client_list) != 0)
	{
		fprintf(stderr, "ERROR: Failed to grab mutex lock in acitveClientRemove: %s", strerror(errno));
        return -1;
	}

	// find position in list in which entry matches
	for(int i = 0; i < client_count; i++)
	{
		if(strcmp(entry.client_name, client_list[i].client_name) == 0 && entry.client_port == client_list[i].client_port)
		{
			matched_pos = i;
			break;
		}
	}

	// update client list if match was previously found
	if(matched_pos != -1)
	{
		client_count--;

		// move last element in list to the opened slot
		memcpy(client_list + matched_pos, client_list + client_count, sizeof(entry));

		printf("Client \"%s:%d\" left server. Server capacity at %d/%d clients\n", entry.client_name, entry.client_port, client_count, MAXCLIENTS);
	}
	else
	{
		fprintf(stderr, "WARN: Called activeClientRemove on client (%s:%d) which was not found in client list\n", entry.client_name, entry.client_port);
        return -1;
	}

	// release lock for client list
	if(pthread_mutex_unlock(&lock_client_list) != 0)
	{
		fprintf(stderr, "ERROR: Failed to release mutex lock in acitveClientRemove: %s", strerror(errno));
        return -1;
	}

	return error;
}

/// montitors clients who have not interacted with server for a certain timeout interval
void* timeoutThreadFun(void* arg)
{
	ClientEntry timed_out_list[MAXCLIENTS];
	int timed_out_count, i;
	time_t curr_time;
	
	while(1)
	{
		timed_out_count = 0;
		
		// grab lock for client list
		if(pthread_mutex_lock(&lock_client_list) != 0)
		{
			fprintf(stderr, "ERROR: Failed to grab mutex lock in timeout thread: %s", strerror(errno));
			exit(EXIT_FAILURE);
		}

		// grab current time
		time(&curr_time);

		// check for timed out clients
		for(i = 0; i < client_count; i++)
		{
			if((curr_time - client_list[i].last_checkin_sec) > TIMEOUT_DURATION_SEC)
			{
				strcpy(timed_out_list[timed_out_count].client_name, client_list[i].client_name);
				timed_out_list[timed_out_count].client_port = client_list[i].client_port;

				timed_out_count++;
			}
		}

		// release lock for client list
		if(pthread_mutex_unlock(&lock_client_list) != 0)
		{
			fprintf(stderr, "ERROR: Failed to release mutex lock in timeout thread: %s. Killing server", strerror(errno));
			exit(EXIT_FAILURE);
		}
	
		// remove timed out clients
		for(i = 0; i < timed_out_count; i++)
		{
			printf("Client \"%s:%d\" timed out\n", timed_out_list[i].client_name, timed_out_list[i].client_port);
			activeClientRemove(timed_out_list[i]);
		}

		// sleep until next check
		sleep(1);
	}
}
