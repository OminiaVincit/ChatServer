/*
 * Network computing
 *		Last assignment: building chat server
 *		ServerUtils.h
 *
 *	Written by Tran Quoc Hoan
 *		with great help from:
 *			(1) http://zobayer.blogspot.jp/2013/06/socket-programming-in-c-pthread.html
 *			(2) https://www.ibm.com/developerworks/library/l-posix1/
*/

#define SELFIP "127.0.0.1"
#define PORT "22609"  /* My port to listen on, in test server = uuid + 21600 */

#define LISTENQ 10  /* Default pass to listen */
#define MAXBUF 1024
#define EXIT_FAILURE 1

#define TRUE 1
#define FALSE 0

#define NAMELEN 20
#define MAXCLIENTS 10
#define COMMANDLEN 10

struct MESS {
	char command[ COMMANDLEN ];
	char clname[ NAMELEN ];
	char buff[ MAXBUF ];
};

struct THREADINFO {
	pthread_t pt_id;
	int s_fd; // socket num
	char clname[ NAMELEN ];
};

struct CLIENTNODE {
	struct THREADINFO thr_inf;
	struct CLIENTNODE *next;
};


struct CLIENTLIST {
	struct CLIENTNODE *head;
	int size;
};

void ListInit( struct CLIENTLIST *cl_list );

int ListInsert( struct CLIENTLIST *cl_list, struct THREADINFO *thread_info );

int ListDelete( struct CLIENTLIST *cl_list, struct THREADINFO *thread_info );

void ListDisplay( const struct CLIENTLIST *cl_list );

void *ClientHandle( void *arg );

/* Error handling */
void ErrHandle( const char * msg );


