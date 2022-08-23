#pragma once

#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <string>

class StateMachine {
	public:
	StateMachine();
	~StateMachine();

	const char* GetState( std::string );
	void SetState( std::string, std::string );
	void SetState( std::string, char* );

	void Interpret( char* buffer );
	void ReadSocket();

	std::string message;

	std::vector<std::string> StateNames;
	std::vector<std::string> StateValues;

	private:

	std::string& LocateState( std::string );
};