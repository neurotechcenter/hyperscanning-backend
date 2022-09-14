#include "client.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Client::Client( int sock, int to ) {
	connection = sock;
	timeout = to;
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
		} else if ( result < 0 ) {
			std::cout << "Error reading socket: " << errno << std::endl;
		} else {
			states.Interpret( buffer );
		}
		free( buffer );
		return true;
	}
	return false;
}

bool Client::SendStates( StateMachine otherStates ) {
	if ( send( connection, otherStates.message.c_str(), otherStates.message.size() + 1, 0 ) < 0 )
		std::cout << "Error writing to socket: " << errno << std::endl;
}

//void StateMachine::ReadSocket() {
//	struct timeval time;
//	time.tv_sec = 0;
//	time.tv_usec = 0;
//
//	FD_ZERO( &readfds );
//	FD_SET( sockfd, &readfds );
//
//	max_fd = sockfd;
//
//	for ( int connection : connections ) {
//		FD_SET( connection, &readfds );
//		if ( connection > max_fd )
//			max_fd = connection;
//	}
//
//	while ( select( max_fd + 1, &readfds, NULL, NULL, &time ) > 0 ) {
//		if ( FD_ISSET( sockfd, &readfds ) ) {
//			std::cout << "New connection..." << std::endl;
//
//			int connection = accept( sockfd, ( struct sockaddr* )&sockaddr, ( socklen_t* )&addrlen );
//			if ( connection < 0 ) {
//				std::cout << "Failed to grab connection" << std::endl;
//			} else {
//				std::cout << "Connected to " << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << std::endl;
//				connections.push_back( connection );
//			}
//		}
//
//		for ( int connection : connections ) {
//			if ( FD_ISSET( connection, &readfds ) ) {
//				char* buffer = ( char* ) malloc( bufferSize * sizeof( char ) );
//				if ( read( connection, buffer, bufferSize ) == 0 ) {
//					getpeername( connection, ( struct sockaddr* )&sockaddr, ( socklen_t* )&addrlen );
//					std::cout << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << " disconnected" << std::endl;
//					close( connection );
//					connection = 0;
//				} else {
//					Interpret( buffer );
//					free( buffer );
//				}
//			}
//		}
//
//		time.tv_sec = 0;
//		time.tv_usec = 0;
//
//		FD_ZERO( &readfds );
//		FD_SET( sockfd, &readfds );
//
//		max_fd = sockfd;
//
//		for ( int connection : connections ) {
//			FD_SET( connection, &readfds );
//			if ( connection > max_fd )
//				max_fd = connection;
//		}
//	}
//}
//
