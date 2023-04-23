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
	Game( Port p, std::string ps ) : port( p ), params( ps ) {
		signal( SIGPIPE, SIG_IGN );
	}

	Port port;
	std::string params;
	std::vector<Client*> clients;
	StateMachine masterStates = StateMachine();

#define each_client for ( auto client : clients )

	bool Connect( Client* client );

	StateMachine Loop();

	bool Update();
};
