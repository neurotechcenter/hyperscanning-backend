#pragma once

#include "statemachine.h"
#include "port.h"
#include "client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

class Game {
	public:
	Game( Port p, std::string ps );

	std::string params;

#define each_client for ( auto client : clients )

	bool Connect( Client* client );
	void ConnectClient( Client* client );
	void AddClient( Client* client );

	StateMachine Loop();

	bool ReadClients( );
	void Reconcile( );
	bool SendToClients( );
	bool Update();

	void ValidateStates( Client* client );

	void SetState( std::string name, std::string value );
	const char* GetState( std::string name );

	private:
	StateMachine* tracker = nullptr;
	StateMachine masterStates = StateMachine();
	std::vector<Client*> clients;
	std::string sharedStates;
	Port port;
};
