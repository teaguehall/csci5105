#include "comms.h"
#include <stdio.h>

int *
join_1_svc(char *arg1, int arg2,  struct svc_req *rqstp)
{
	static int  result;

	printf("Client joined server!\n");

	return &result;
}

int *
leave_1_svc(char *arg1, int arg2,  struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
subscribe_1_svc(char *arg1, int arg2, char *arg3,  struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
unsubscribe_1_svc(char *arg1, int arg2, char *arg3,  struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
publish_1_svc(char *arg1, int arg2, char *arg3,  struct svc_req *rqstp)
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

	/*
	 * insert server code here
	 */

	return &result;
}
