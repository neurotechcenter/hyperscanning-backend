#pragma once

#include "statemachine.h"

class Client {
	public:
	Client( int, int );

	bool GetUpdatedStates( StateMachine& );
	bool SendStates( StateMachine );

	private:
	int timeout;
	int connection;
	sockaddr_in sockaddr;

	std::vector<int> connections;
	int max_fd;
	fd_set readfds;

	int addrlen = sizeof( sockaddr );
	int bufferSize = 1025;
};
