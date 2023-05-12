#include "client.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Client::Client( int sock, int to, std::string ip, int p ) {
	connection = sock;
	timeout = to;
	ip_address = ip;
	port = p;
	states = new StateMachine();
	stateChanges = new StateMachine();
}

bool Client::GetUpdatedStates() {
	struct timeval time;
	time.tv_sec = 0;
	time.tv_usec = timeout;

	FD_ZERO( &readfds );
	FD_SET( connection, &readfds );

	while ( select( connection + 1, &readfds, NULL, NULL, NULL ) > 0 ) {
		char* buffer = ( char* ) malloc( bufferSize * sizeof( char ) );
		size_t result = read( connection, buffer, bufferSize );
		if ( result == 0 ) {
			std::cout << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << " disconnected" << std::endl;
			close( connection );
			connection = 0;
			return false;
		} else if ( result < 0 ) {
			std::cout << "Error reading socket: " << errno << std::endl;
		} else {
			states->Interpret( buffer, stateChanges );
		}
		free( buffer );
		return true;
	}
	return false;
}

bool Client::SendStates( StateMachine otherStates ) {
	std::string message = otherStates.GetMessage();
	size_t size = message.size() + 1;
	if ( send( connection, &size, sizeof( size_t ), MSG_NOSIGNAL ) < 0 || send( connection, message.c_str(), size, MSG_NOSIGNAL ) < 0 ) {
		std::cout << "Error writing to socket: " << errno << std::endl;
		return false;
	}
	return true;
}

bool Client::Matches( Client* o ) {
	return ( ip_address == o->ip_address && port == o->port );
}
