#pragma once

#include <vector>
#include "client.h"

class Port {
	public: 
	Port( int, int );

	Client* WaitForClient();
	Client* CheckForClient();

	private:
	std::vector<Client*> connections;

	int sockfd;
	sockaddr_in sockaddr;

	int max_fd;
	fd_set readfds;

	int addrlen = sizeof( sockaddr );

	int timeout;

	int port;
};
