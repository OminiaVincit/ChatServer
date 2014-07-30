
 * Network computing
 *		Last assignment: building chat server
 *
 *	Written by Tran Quoc Hoan
 *		with great help from:
 *			(1) http://zobayer.blogspot.jp/2013/06/socket-programming-in-c-pthread.html
 *			(2) https://www.ibm.com/developerworks/library/l-posix1/

・チャットサーバの作成：　thread 利用
・IPv4 / IPv6 対応
・Multi users chat up to MAXCLIENTS=20
・Commands (client)
    login: 				login as Anonymous user
    login username: 	login as username
    name username: 		change nickname to username
    send message: 		send message to all users in chat serverr
    uni dest message: 	send message to only destination server
    logout: 			logout of chat
    exit: 				exit client service