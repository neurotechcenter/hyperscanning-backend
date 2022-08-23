#include "socket.h"

Socket::Socket() {
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		return -1;
	}

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons( 9999 );

	if ( bind( sockfd, ( struct sockaddr* )&sockaddr, sizeof( sockaddr ) ) < 0 ) {
		std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
		return -1;
	}

	if ( listen( sockfd, 10 ) < 0 ) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		return -1;
	}

	for (;;) {
		FD_ZERO( &readfds );
		FD_SET( sockfd, &readfds );

		max_fd = sockfd;

		for ( int connection : connections ) {
			FD_SET( connection, &readfds );
			if ( connection > max_fd )
				max_fd = connection;
		}

		select( max_fd + 1, &readfds, NULL, NULL, NULL );

		if ( FD_ISSET( sockfd, &readfds ) ) {
			std::cout << "New connection..." << std::endl;

			int connection = accept( sockfd, ( struct sockaddr* )&sockaddr, ( socklen_t* )&addrlen );
			if ( connection < 0 ) {
				std::cout << "Failed to grab connection" << std::endl; 
			} else {
				std::cout << "Connected to " << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << std::endl;
				connections.push_back( connection );
			}
		}

		for ( int connection : connections ) {
			if ( FD_ISSET( connection, &readfds ) ) {
				char* buffer = ( char* ) malloc( 1025 * sizeof( char ) );
				if ( read( connection, buffer, 1025 ) == 0 ) {
					getpeername( connection, ( struct sockaddr* )&sockaddr, ( socklen_t* )&addrlen );
					std::cout << inet_ntoa( sockaddr.sin_addr ) << ":" << ntohs( sockaddr.sin_port ) << " disconnected" << std::endl;
					close( connection );
					connection = 0;
				} else {
					OnMessage( buffer )
				}
			}
		}
	}
}
