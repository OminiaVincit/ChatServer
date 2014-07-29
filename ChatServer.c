/*
 * Network computing
 *		Last assignment: building chat server
 *		ChatServer.c
 *
 *	Written by Tran Quoc Hoan
 *		with great help from:
 *			(1) http://zobayer.blogspot.jp/2013/06/socket-programming-in-c-pthread.html
 *			(2) https://www.ibm.com/developerworks/library/l-posix1/
*/

#include <pthread.h>
#include <stdio.h>
#include <string.h>		/* for memset() function */
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <netdb.h>

#include "ServerUtils.h"

struct CLIENTLIST client_list;
pthread_mutex_t cl_mutex;

void *ClientHandle( void* arg ) {
	struct THREADINFO thr_inf = *(struct THREADINFO *)arg;
	struct MESS mss;
	struct CLIENTNODE *node;
	while( 1 ) {
		memset( &mss, 0, sizeof( struct MESS ) );
		if ( recv( thr_inf.s_fd, (void *)&mss, sizeof( struct MESS),0 ) < 0 ) {
			fprintf( stderr, "Connection lost from [%d] %s ... \n", thr_inf.s_fd, thr_inf.clname );
			pthread_mutex_lock( &cl_mutex );
			ListDelete( &client_list, &thr_inf );
			ListDisplay( &client_list );
			pthread_mutex_unlock( &cl_mutex );
			break;
		}

		printf( "[%d] %s %s %s\n", thr_inf.s_fd, mss.clname, mss.command, mss.buff );
		
		// Set nickname
		if ( !strcmp( mss.command, "name" ) ) {
			pthread_mutex_lock( &cl_mutex );
			node = client_list.head;
			printf( "Change nickname to %s\n", mss.clname );
			while ( node != NULL ) {
				if ( node->thr_inf.s_fd == thr_inf.s_fd ){
					strcpy( node->thr_inf.clname, mss.clname );
					strcpy( thr_inf.clname, mss.clname );
					break;
				}
				node = node->next;
			}
			
			pthread_mutex_unlock( &cl_mutex );
			ListDisplay( &client_list );
		}
		else if ( !strcmp( mss.command, "send" ) ) {
			pthread_mutex_lock( &cl_mutex );
			node = client_list.head;
			
			while( node != NULL ) {
				struct MESS send_mss;
				memset( &send_mss, 0, sizeof( struct MESS ) );
				if ( node->thr_inf.s_fd != thr_inf.s_fd ) {
					// send messages to all other clients
					strcpy( send_mss.command, "message" );
					strcpy( send_mss.clname, mss.clname );
					strcpy( send_mss.buff, mss.buff );
					send( node->thr_inf.s_fd, (void *)&send_mss, sizeof(struct MESS), 0 );
				};
				node = node->next;
			}
			pthread_mutex_unlock( &cl_mutex );
		}
		else if ( !strcmp( mss.command, "uni" ) ) {
			int i = 0;
			char dst[ NAMELEN ];
			// get destination nickname
			while ( mss.buff[i] != ' ') i++;
			mss.buff[i++] = 0;
			strcpy( dst, mss.buff );
			pthread_mutex_lock( &cl_mutex );

			node = client_list.head;
			while ( node != NULL ) {
				if ( strcmp( dst, node->thr_inf.clname ) == 0 ) {
					struct MESS send_mss;
					memset( &send_mss, 0, sizeof( struct MESS ) );
					if ( node->thr_inf.s_fd != thr_inf.s_fd ) {
						// send messages to one client
						strcpy( send_mss.command, "message" );
						strcpy( send_mss.clname, mss.clname );
						strcpy( send_mss.buff, &mss.buff[i] );
						send( node->thr_inf.s_fd, (void *)&send_mss, sizeof(struct MESS), 0 );
					};
				}
				node = node->next;
			}
			pthread_mutex_unlock( &cl_mutex );
		}
		else if ( !strcmp( mss.command, "exit") || !strcmp(mss.command, "") ) {
			printf( "[%d] %s has disconnected!\n", thr_inf.s_fd, thr_inf.clname );
			pthread_mutex_lock( &cl_mutex );
			ListDelete( &client_list, &thr_inf );
			ListDisplay( &client_list );
			pthread_mutex_unlock( &cl_mutex );
			break;
		}
		else {
			fprintf( stderr, "Do something... \n" );
		}
	}

	// close socket
	close( thr_inf.s_fd );
	return NULL;
}

int main ( int argc, char* argv[] ) {
	
	// Usage
	char* ip_addr;
	if ( argc != 2 ) {
		fprintf ( stderr, "Usage: %s <Server Name or IPaddress>\n", argv[0] );
		fprintf ( stderr, "Using local host as default sever in this case. \n" );
		ip_addr = SELFIP;
	} else {
		ip_addr = argv[1];
	}
	
	// Address information
	int listenfd;
	pthread_t thread;
	struct addrinfo servaddr, *rcv;
	int reuseaddr = 1; // true
	
	// Get the address info
	memset( &servaddr, 0, sizeof( struct addrinfo ) );
	
	// Respond with both IPv4 and IPv6
	if ( strchr( ip_addr, ':') == NULL ) servaddr.ai_family = AF_INET;
	else servaddr.ai_family = AF_INET6;

	servaddr.ai_socktype = SOCK_STREAM;
		
	if ( getaddrinfo( ip_addr, PORT, &servaddr, &rcv ) != 0 ) 
		ErrHandle( "getaddrinfo" );
	
	// Create the socket
	listenfd = socket( rcv->ai_family, rcv->ai_socktype, rcv->ai_protocol );
	if ( listenfd == -1 ) ErrHandle( "socket" );
	
	// Enable the socket to reuse the address immediately
	if ( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof( int ) ) == -1 ) 
		ErrHandle( "setsockopt" );
	
	// Bind to the address
	if ( bind( listenfd, rcv->ai_addr, rcv->ai_addrlen ) == -1 ) 
		ErrHandle( "bind" );
	
	freeaddrinfo( rcv );
	
	// Listen 
	if ( listen( listenfd, LISTENQ ) == -1 ) 
		ErrHandle( "listen" );
	
	// Initialize linked list
	ListInit( &client_list );

	// Initiate mutex
	pthread_mutex_init( &cl_mutex, NULL );

	// Thread for command 

	// Main loop
	while (1) {
	  struct sockaddr_in comming_addr;
	  size_t size = sizeof( struct sockaddr_in );
	  int confd = accept( listenfd, (struct sockaddr*)&comming_addr, (socklen_t*)&size );
	  
	  if ( confd == -1 ) ErrHandle( "accept" );
	  else {
	  	printf("Comming connection from [%s] on port [%d] \r\n", inet_ntoa(comming_addr.sin_addr), htons(comming_addr.sin_port));

	  	// client list is full
	  	if ( client_list.size == MAXCLIENTS ) {
	  		fprintf( stderr, "Connection is full, request rejected! \n" );
	  		continue;
	  	};
	  	printf( "Connection established! \n" );
	  	struct THREADINFO new_client;
	  	new_client.s_fd = confd;
	  	strcpy( new_client.clname, "Anonymous" );
	  	pthread_mutex_lock( &cl_mutex );
	  	ListInsert( &client_list, &new_client );
	  	ListDisplay( &client_list );
	  	pthread_mutex_unlock( &cl_mutex );
	  	if ( pthread_create( &new_client.pt_id, NULL, ClientHandle, (void *)&new_client ) != 0 ) {
		  perror( "pthread_create" );
	  	}
	  }
	}

	// close socket
	close( listenfd );

	return 1;
}
