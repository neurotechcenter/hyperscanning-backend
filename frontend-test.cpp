#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

int main() {
	std::cout << "Opening socket" << std::endl;
	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd == -1 ) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		return -1;
	}

	sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons( 9999 );

	if ( inet_pton( AF_INET, "10.138.1.182", &serv_addr.sin_addr ) <= 0 ) {
		std::cout << "Invalid address / Address not supported" << std::endl;
		return -1;
	}
	
	int clientfd;
	if ( ( clientfd = connect( sockfd, ( struct sockaddr* )&serv_addr, sizeof( serv_addr ) ) ) < 0 ) {
		std::cout << "Connection Failed..." << std::endl;
		return -1;
	}

	int n = 0;
	for ( ;; ) {

		std::string message( "bob" );
		message.push_back( '\0' );
		message.push_back( ( char ) sizeof( n ) );
		message += std::string( ( char* )( &n ), sizeof( n ) ).c_str();

		send( sockfd, message.c_str(), message.size(), 0 );

		std::cout << message << std::endl;

		char* buffer = ( char* )malloc( 1025 * sizeof( char ) );
		read( sockfd, buffer, 1025 );
		n++;

		std::cout << "N: " << n << std::endl;
	}
}
