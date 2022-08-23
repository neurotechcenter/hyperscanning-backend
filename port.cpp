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

Client Port::WaitForClient() {
	int connection = accept( sockfd, ( struct sockaddr* )&sockaddr, ( socklen_t* )&addrlen );
	if ( connection < 0 ) {
		std::cout << "Failed to grab connection" << std::endl; 
	} else {
		std::cout << "Connected to " << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << std::endl;
		Client client( connection, timeout );
		connections.push_back( client );
		return client;
	}
}

bool Port::GetUpdatedStates( StateMachine& states ) {
	bool all = true;
	for ( auto connection: connections ) {
		if ( !connection.GetUpdatedStates( states ) )
			all = false;
	}
	return all;
}

bool Port::SendStates( StateMachine states ) {
	bool all = true;
	for ( auto connection: connections ) {
		if ( !connection.SendStates( states ) )
			all = false;
	}

	states.message = "";

	return all;
}
