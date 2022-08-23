#pragma once

#include <sys/socket.h>
#include <vector>

class Socket {
	public:
	Socket();
	~Socket();


	private:
	int sockfd;
	sockaddr_in sockaddr;

	std::vector<int> connections;
	int max_fd;
	fd_set readfds;

	auto addrlen = sizeof( sockaddr );
}
