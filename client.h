#pragma once

#include "statemachine.h"

class Client {
	public:
	Client( int, int, std::string, int );

	bool GetUpdatedStates();
	bool SendStates( StateMachine );

	std::string ip_address;
	int port;

	StateMachine states;
	int connection;
	char ClientNumber;

	private:
	int timeout;
	sockaddr_in sockaddr;


	std::vector<int> connections;
	int max_fd;
	fd_set readfds;

	int addrlen = sizeof( sockaddr );
	int bufferSize = 1025;

};
