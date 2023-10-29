#include "port.h"
#include "client.h"
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

Port::Port( int p, int to ) {
	timeout = to;
	port = p;
	open = true;

	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		open = false;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons( port );

	if ( bind( sockfd, ( struct sockaddr* )&sockaddr, sizeof( sockaddr ) ) < 0 ) {
		std::cout << "Failed to bind to port " << port << ". errno: " << errno << std::endl;
		open = false;
	}

	if ( listen( sockfd, 10 ) < 0 ) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		open = false;
	}
	
}

Client* Port::WaitForClient() {
	int connection = accept( sockfd, ( struct sockaddr* )&sockaddr, ( socklen_t* )&addrlen );
	if ( connection < 0 ) {
		std::cout << "Failed to grab connection" << std::endl; 
	} else {
		std::cout << "Connected to " << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << std::endl;

		// Version
		size_t sbuffer;
		if ( read( connection, &sbuffer, sizeof( size_t ) ) <= 0 )
			std::cout << "error: " << errno << std::endl;
		char* buffer = ( char* )malloc( sbuffer );
		if ( read( connection, buffer, sbuffer ) <= 0 ) {
			std::cout << "Error when reading Client ID: " << errno << std::endl;
		}
		std::string version = std::string( buffer, sbuffer );
		free( buffer );

		// Client ID
		std::string id;
		if ( read( connection, &sbuffer, sizeof( size_t ) ) <= 0 )
			std::cout << "error: " << errno << std::endl;
		if ( sbuffer ) {
			buffer = ( char* )malloc( sbuffer );
			if ( read( connection, buffer, sbuffer ) <= 0 ) {
				std::cout << "Error when reading Client ID: " << errno << std::endl;
			}
			id = std::string( buffer, sbuffer );
		} else {
			id = inet_ntoa( sockaddr.sin_addr );
		}


		// Session ID
		if ( read( connection, &sbuffer, sizeof( size_t ) ) <= 0 )
			std::cout << "error: " << errno << std::endl;
		buffer = ( char* )malloc( sbuffer );
		if ( read( connection, buffer, sbuffer ) <= 0 ) {
			std::cout << "Error when reading Client ID: " << errno << std::endl;
		}
		std::string sid = std::string( buffer, sbuffer );

		std::cout << "Client Version: " << version << std::endl;
		std::cout << "Client ID: " << id << std::endl;
		std::cout << "Session ID: " << sid << std::endl;

		Client* client = new Client( connection, inet_ntoa( sockaddr.sin_addr ), ntohs( sockaddr.sin_port ), version, id, sid );
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

void Port::Close() {
	close( sockfd );
	for ( auto c : connections )
		close( c->connection );
}
