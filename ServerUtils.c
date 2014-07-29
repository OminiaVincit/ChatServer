/*
 * Network computing
 *		Last assignment: building chat server
 *		ServerUtils.c
 *
 *	Written by Tran Quoc Hoan
 *		with great help from:
 *			(1) http://zobayer.blogspot.jp/2013/06/socket-programming-in-c-pthread.html
 *			(2) https://www.ibm.com/developerworks/library/l-posix1/
*/

#include <stdio.h>
#include <string.h>		/* for memset() function */
#include <stdlib.h>

#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#include "ServerUtils.h"

/* Error handling */
void ErrHandle( const char * msg ) {
	perror( msg );
	exit ( EXIT_FAILURE );
}

void ListInit( struct CLIENTLIST *cl_list ) {
	cl_list->head = NULL;
	cl_list->size = 0;
}

int ListInsert( struct CLIENTLIST *cl_list, struct THREADINFO *thread_info ) {
	if ( cl_list->size == MAXCLIENTS ) return 0;
	struct CLIENTNODE* clnode = (struct CLIENTNODE *)malloc( sizeof( struct CLIENTNODE ));
	clnode->thr_inf = *thread_info;
	clnode->next = cl_list->head;
	cl_list->head = clnode;
	cl_list->size++;
	return 1;
};

int ListDelete( struct CLIENTLIST *cl_list, struct THREADINFO *thread_info ) {
	struct CLIENTNODE *cur, *tmp;
	cur = cl_list->head;
	if ( cur == NULL ) return 0;
	if ( thread_info->s_fd == cur->thr_inf.s_fd ) {
		cl_list->head = cur->next;
		free(cur);
		cl_list->size--;
		return 1;
	}
	while ( cur->next != NULL ) {
		if ( thread_info->s_fd == cur->next->thr_inf.s_fd ) {
			tmp = cur->next;
			cur->next = tmp->next;
			free(tmp);
			cl_list->size--;
			return 1;
		}
		cur = cur->next;
	}
	return 0;
};

void ListDisplay( const struct CLIENTLIST *cl_list ) {
	struct CLIENTNODE *cur = cl_list->head;
	struct THREADINFO thr_inf;
	printf ("Number of connections: %d \n", cl_list->size );
	while ( cur != NULL ) {
		thr_inf = cur->thr_inf;
		printf( "	Client %i [%d] %s \n", thr_inf.s_fd - 3, thr_inf.s_fd, thr_inf.clname );
		cur = cur->next;
	}
}
