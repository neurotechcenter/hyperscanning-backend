#pragma once

#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <string>

class StateMachine {
	public:
	StateMachine();

	const char* GetState( std::string );
	bool SetState( std::string, std::string );

	void Interpret( const char* buffer, StateMachine* );

	std::string GetMessage();

	private:

	std::string& LocateState( std::string );

	std::vector<std::string> StateNames;
	std::vector<std::string> StateValues;

	std::vector<std::string> ChangedStates;
	std::vector<std::string> ChangedValue;
};
