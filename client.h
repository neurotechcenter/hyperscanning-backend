#pragma once

#include "statemachine.h"

class Client {
	public:
	Client( int, int );

	bool GetUpdatedStates();
	bool SendStates( StateMachine );

	StateMachine states;
	int connection;
	private:
	int timeout;
	sockaddr_in sockaddr;

	std::vector<int> connections;
	int max_fd;
	fd_set readfds;

	int addrlen = sizeof( sockaddr );
	int bufferSize = 1025;

};
