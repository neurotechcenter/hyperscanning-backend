#pragma once

#include "statemachine.h"
#include "port.h"
#include "client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

void connect_client( Client* client, std::string params, char n ) {
	size_t size = params.size();
	send( client->connection, &size, sizeof( params.size() ), 0 );
	send( client->connection, params.c_str(), params.size(), 0 );
	client->ClientNumber = n;
	std::string s = "ClientNumber";
	s.push_back( '\0' );
	s.push_back( 1 );
	s.push_back( client->ClientNumber );
	s.push_back( '\0' );
	send( client->connection, s.c_str(), s.size(), 0 );
	client->states.message.clear();
}

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

	void Connect( Client* client ) {
		if ( client ) {
			connect_client( client, params, clients.size() );
			clients.push_back( client );
		} else {
			std::cout << "Client does not exist" << std::endl;
		}
	}

	StateMachine Loop() {
		for (;;) {
			if ( !Update() )
				return masterStates;
		}
	}

	bool Update() {
		std::cout << "Getting updated states..." << std::endl;
		each_client {
			if ( !client->GetUpdatedStates() ) return false;
		}

		masterStates.message.clear();
		int i = 0;
		each_client {
			std::cout << "Updating client " << i++ << "'s states..." << std::endl;
			//std::cout << "Message: " << std::endl;
			//std::cout << client->states.message << std::endl;
			client->states.message.push_back( 0 );
			masterStates.Interpret( client->states.message.c_str() );
			client->states.message.clear();
		}

		std::cout << "Sending updated states..." << std::endl;
		std::cout << "Time: " << std::endl;
		std::cout << std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() ) << std::endl;

		each_client {
			if ( !client->SendStates( masterStates ) ) return false;
		}

		std::cout << "isReadyStart0: " << ( int ) *masterStates.GetState( "isReadyStart0" ) << std::endl;
		std::cout << "isReadyStart1: " << ( int ) *masterStates.GetState( "isReadyStart1" ) << std::endl;

		return true;
	}
};
