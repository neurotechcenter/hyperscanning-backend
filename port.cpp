#include "port.h"
#include "client.h"
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Port::Port( int p, int to ) {
	timeout = to;
	port = p;

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons( port );

	if ( bind( sockfd, ( struct sockaddr* )&sockaddr, sizeof( sockaddr ) ) < 0 ) {
		std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
	}

	if ( listen( sockfd, 10 ) < 0 ) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
	}
}

Client* Port::WaitForClient() {
	int connection = accept( sockfd, ( struct sockaddr* )&sockaddr, ( socklen_t* )&addrlen );
	if ( connection < 0 ) {
		std::cout << "Failed to grab connection" << std::endl; 
	} else {
		std::cout << "Connected to " << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << std::endl;
		Client* client = new Client( connection, timeout, inet_ntoa( sockaddr.sin_addr ), ntohs( sockaddr.sin_port ) );
		connections.push_back( client );
		return client;
	}
}

Client* Port::CheckForClient() {
	fd_set fds;
	FD_ZERO( &fds );
	FD_SET( sockfd, &fds );

	struct timeval to = { 0, 0 };

	select( sockfd, &fds, NULL, NULL, &to );

	if ( FD_ISSET( sockfd, &fds ) != 0 )
		return WaitForClient();
	return nullptr;
}

