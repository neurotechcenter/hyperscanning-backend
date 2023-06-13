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

	Port port;
	std::string params;
	std::string sharedStates;
	std::vector<Client*> clients;
	StateMachine masterStates = StateMachine();

#define each_client for ( auto client : clients )

	bool Connect( Client* client );
	void ConnectClient( Client* client );
	void AddClient( Client* client );

	StateMachine Loop();

	bool Update();

	void ValidateStates( Client* client );
};
