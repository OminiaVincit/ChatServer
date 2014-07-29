/*
 * Network computing
 *		Last assignment: building chat server
 *		ClientUtils.h
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

#define FAILURE -1

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
};

struct USER {
	int conn_flag; // connection flag
	int s_fd;	   // correspondence socket num	
	char clname[ NAMELEN ];
};

//
int ServerConnect( const char* addr );

void Login( struct USER *usr, const char* addr );

void Logout( struct USER *usr );

void *GetMss( void *arg );

void Broadcast( const struct USER *usr, char *msg );

void Unicast( struct USER *src, const char *dst, char *msg );

void SetName( struct USER* usr );

// Error handling
int ErrHandle( const char * msg );

int CheckConnect( const struct USER* usr );
