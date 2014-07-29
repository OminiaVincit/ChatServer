/*
 * Network computing
 *		Last assignment: building chat server
 *		ChatClient.c
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

#include "ClientUtils.h"

#define LINEBUFF 2048

int main ( int argc, char* argv[] ) {
	struct USER myself;
	char line[ LINEBUFF ];
	char *p;

	char* ip_addr;
	if ( argc != 2 ) {
		fprintf ( stderr, "Usage: %s <Server Name or IPaddress>\n", argv[0] );
		fprintf ( stderr, "Using local host as default sever in this case. \n" );
		ip_addr = SELFIP;
	} else {
		ip_addr = argv[1];
	}

	int namelen;
	memset( &myself, 0, sizeof( struct USER ) );
	while ( fgets( line, sizeof( line ), stdin ) ) {
		
		if ((p = strchr( line, '\n')) != NULL ) *p = '\0';
		
		// Exit command
		if ( !strncmp( line, "exit", 4)) {
			Logout( &myself );
			break;
		}

		// Login command
		if ( !strncmp( line, "login", 5) ) {
			char *tk = strtok( line, " " );
			tk = strtok( 0, " " );
			memset( myself.clname, 0, sizeof( char ) * NAMELEN );
			if ( tk != NULL ) {
				namelen = strlen( tk );
				if ( namelen > NAMELEN ) tk[ NAMELEN ] = 0;
				strcpy( myself.clname, tk );
			} 
			else strcpy( myself.clname, "Anonymous" );
			Login( &myself, ip_addr );
		}
		else if ( !strncmp( line, "name", 4 ) ) {
			char *tk = strtok( line, " " );
			tk = strtok( 0, " " );
			memset( myself.clname, 0, sizeof( char ) * NAMELEN );
			if ( tk != NULL ) {
				namelen = strlen( tk );
				if ( namelen > NAMELEN ) tk[ NAMELEN ] = 0;
				strcpy( myself.clname, tk );
				SetName( &myself );
			} 
		}
		else if ( !strncmp( line, "uni", 3) ) {
			char *tk = strtok( line, " ");
			char temp[ NAMELEN ];
			tk = strtok( 0, " " );

			memset( temp, 0, sizeof(char) *NAMELEN );
			if ( tk != NULL ){
				namelen = strlen( tk );
				if ( namelen > NAMELEN ) tk[ NAMELEN ] = 0;
				strcpy( temp, tk );
				while ( *tk ) tk++;
				tk++;
				while ( (*tk) && (*tk <= ' ') ) tk++;
				Unicast( &myself, temp, tk );
			}
		}
		else if ( !strncmp( line, "send", 4) ) {
			Broadcast( &myself, &line[5] );
		}
		else if ( !strncmp( line, "logout", 6)) {
			Logout( &myself );
		}
		else fprintf( stderr, "Unknown command???\n" );
	}
	return 1;
}
