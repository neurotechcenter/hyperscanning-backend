#pragma once

#include "statemachine.h"

class Client {
	#define MSG_NOSIGNAL 0
	
	public:
	Client( int, std::string, int, std::string, std::string, std::string );

	bool GetUpdatedStates();
	bool SendStates( StateMachine );

	std::string ip_address;
	int port;

	std::string version;
	std::string id;
	std::string sid;

	int connection;
	char ClientNumber;

	StateMachine* states = nullptr;
	StateMachine* stateChanges = nullptr;

	bool Matches( Client* o );

	private:
	sockaddr_in sockaddr;


	int max_fd;
	fd_set readfds;

	int addrlen = sizeof( sockaddr );
	int bufferSize = 1025;

};
