/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "communicate.h"


void
communicate_prog_1(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	char *join_1_ip;
	int join_1_port;
	int  *result_2;
	char *leave_1_ip;
	int leave_1_port;
	int  *result_3;
	char *subscribe_1_ip;
	int subscribe_1_port;
	char *subscribe_1_article;
	int  *result_4;
	char *unsubscribe_1_ip;
	int unsubscribe_1_port;
	char *unsubscribe_1_article;
	int  *result_5;
	char *publish_1_ip;
	int publish_1_port;
	char *publish_1_article;
	int  *result_6;

#ifndef	DEBUG
	clnt = clnt_create (host, COMMUNICATE_PROG, COMMUNICATE_VERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = join_1(join_1_ip, join_1_port, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_2 = leave_1(leave_1_ip, leave_1_port, clnt);
	if (result_2 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_3 = subscribe_1(subscribe_1_ip, subscribe_1_port, subscribe_1_article, clnt);
	if (result_3 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_4 = unsubscribe_1(unsubscribe_1_ip, unsubscribe_1_port, unsubscribe_1_article, clnt);
	if (result_4 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_5 = publish_1(publish_1_ip, publish_1_port, publish_1_article, clnt);
	if (result_5 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_6 = ping_1(clnt);
	if (result_6 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	communicate_prog_1 (host);
exit (0);
}
