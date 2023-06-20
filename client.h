#pragma once

#include "statemachine.h"

class Client {
	#define MSG_NOSIGNAL 0
	
	public:
	Client( int, int, std::string, int );

	bool GetUpdatedStates();
	bool SendStates( StateMachine );

	std::string ip_address;
	int port;

	int connection;
	char ClientNumber;

	StateMachine* states = nullptr;
	StateMachine* stateChanges = nullptr;

	bool Matches( Client* o );

	private:
	int timeout;
	sockaddr_in sockaddr;


	std::vector<int> connections;
	int max_fd;
	fd_set readfds;

	int addrlen = sizeof( sockaddr );
	int bufferSize = 1025;

};
