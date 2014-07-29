/*
 * Network computing
 *		Last assignment: building chat server
 *		ClientUtils.c
 *
 *	Written by Tran Quoc Hoan
 *		with great help from:
 *			(1) http://zobayer.blogspot.jp/2013/06/socket-programming-in-c-pthread.html
 *			(2) https://www.ibm.com/developerworks/library/l-posix1/
*/

#include <stdio.h>
#include <string.h>		// for memset() function 
#include <stdlib.h>

#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#include "ClientUtils.h"

int ServerConnect( const char* addr ) {

	// Address information
	int listenfd;
	pthread_t thread;
	struct addrinfo servaddr, *rcv;
	int reuseaddr = 1; // true
	
	// Get the address info
	memset( &servaddr, 0, sizeof( struct addrinfo ) );

	// Respond with both IPv4 and IPv6
	if ( strchr( addr, ':') == NULL ) servaddr.ai_family = AF_INET;
	else servaddr.ai_family = AF_INET6;

	servaddr.ai_socktype = SOCK_STREAM;
		
	if ( getaddrinfo( addr, PORT, &servaddr, &rcv ) != 0 ) 
		return ErrHandle( "getaddrinfo" );
	
	// Create the socket
	listenfd = socket( rcv->ai_family, rcv->ai_socktype, rcv->ai_protocol );
	if ( listenfd == -1 ) return ErrHandle( "socket" );
	
	// Enable the socket to reuse the address immediately
	if ( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof( int ) ) == -1 ) 
		return ErrHandle( "setsockopt" );

	if ( connect( listenfd, rcv->ai_addr, rcv->ai_addrlen ) == -1 )
		return ErrHandle( "error connect to server " );
	else {
		printf ("Connected to server at [%s]:[%s] ", addr, PORT );
		return listenfd;
	}
}

void Login( struct USER *usr, const char* addr ) {
	if ( usr->conn_flag) {
		fprintf( stderr, "Still login to server [%s] at port [%s] . \n", addr, PORT );
		return;
	}
	int sockfd = ServerConnect( addr );
	if ( sockfd >= 0 ) {
		usr->conn_flag = 1;
		usr->s_fd = sockfd;
		if ( strcmp( usr->clname, "Anonymous" ) )  {
			// set nickname Anonymous by sending to server
			SetName( usr );
		};

		printf( "Logged in as [%s]\n", usr->clname );
		printf( "Start getting message at [%d].\n", sockfd );
		struct THREADINFO thr_inf;
		pthread_create( &thr_inf.pt_id, NULL, GetMss, (void *)usr );
	} else {
		fprintf(stderr, "Cannot login due to connection's problem! \n" );
	}
}

void Logout( struct USER *usr ) {
	struct MESS mss;
	if ( !CheckConnect( usr ) ) return;
    memset( &mss, 0, sizeof( struct MESS ) );
    strcpy( mss.command, "exit" );
    strcpy( mss.clname, usr->clname );
    send( usr->s_fd, (void *)&mss, sizeof( struct MESS), 0 );
    // change status of connection
    usr->conn_flag = 0;
}

void *GetMss( void *arg ) {
	int getms = 0;
	struct MESS mss;
	struct USER usr = *(struct USER *)arg;

	printf( "Getting message at [%d]...\n", usr.s_fd );
	while ( usr.conn_flag ) {
		getms = recv( usr.s_fd, (void*)&mss, sizeof( struct MESS), 0 );
		if ( !getms ) {
			fprintf( stderr, "Connection lost from server!\n" );
			usr.conn_flag = 0;
			close( usr.s_fd );
			break;
		}
		if ( getms > 0 ) {
			printf("[%s]: %s \n", mss.clname, mss.buff );
		}
		memset( &mss, 0, sizeof( struct MESS ) );
	}
	return NULL;
}

void Broadcast( const struct USER *usr, char *msg ) {
	struct MESS mss;
	if ( !CheckConnect( usr ) ) return ;

	msg[ MAXBUF ] = 0;
	memset( &mss, 0, sizeof( struct MESS ) );
	strcpy( mss.command, "send" );
	strcpy( mss.clname, usr->clname );
	strcpy( mss.buff, msg );

	send( usr->s_fd, (void *)&mss, sizeof(struct MESS), 0 );
}

void Unicast( struct USER *src, const char *dst, char *msg ) {
	struct MESS mss;
	if ( dst == NULL || msg == NULL ) return;
	if ( !CheckConnect( src ) ) return;
	msg[ MAXBUF ] = 0;
	int dstlen = strlen( dst );
	memset( &mss, 0, sizeof( struct MESS ) );
	strcpy( mss.command, "uni" );
	strcpy( mss.clname, src->clname );
	strcpy( mss.buff, dst );
	strcpy( &mss.buff[ dstlen ], " ");
	strcpy( &mss.buff[ dstlen + 1], msg );

	send( src->s_fd, (void *) &mss, sizeof( struct MESS ), 0 );
}

// Set nickname to client
void SetName( struct USER* usr ) {
	struct MESS mss;
	if ( !CheckConnect( usr ) ) return ;
	memset( &mss, 0, sizeof( struct MESS ) );
	strcpy( mss.command, "name" );
	strcpy( mss.clname, usr->clname );

	send( usr->s_fd, (void *)&mss, sizeof( struct MESS ), 0 );
}

// Error handling
int ErrHandle( const char * msg ) {
	perror( msg );
	return FAILURE;
}

int CheckConnect( const struct USER* usr ){
	if ( !usr->conn_flag ) {
		fprintf( stderr, "Not in connected status! \n" );
		return FALSE;
	}
	return TRUE;
}
