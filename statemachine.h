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
	bool SetState( std::string, std::string );
//	void SetState( std::string, std::string, bool );
//	void SetState( std::string, char*, char );

	void Interpret( const char* buffer, StateMachine* );
	//void Interpret( char* buffer );
	void ReadSocket();

	std::string GetMessage();

	void ClearMessage();

	std::string message;

	std::vector<std::string> StateNames;
	std::vector<std::string> StateValues;

	std::vector<std::string> ChangedStates;
	std::vector<std::string> ChangedValue;

	private:

	std::string& LocateState( std::string );
};
