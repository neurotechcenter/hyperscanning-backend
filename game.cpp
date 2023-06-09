#include "game.h"
#include "statemachine.h"
#include "port.h"
#include "client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <signal.h>


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
}

void print_string( std::string in ) {
	for ( auto c : in ) {
		if ( c > 32 ) std::cout << c;
		else std::cout << "\\" << ( int ) c;
	}
	std::cout << std::endl;
}

Game::Game( Port p, std::string ps ) : port( p ), params( ps ) {
	signal( SIGPIPE, SIG_IGN );
}


bool Game::Connect( Client* client ) {
	if ( client ) {
		for ( auto c : clients ) {
			if ( c->Matches( client ) )
				return false;
		}
		connect_client( client, params, clients.size() );
		clients.push_back( client );
	} else {
		std::cout << "Client does not exist" << std::endl;
		return false;
	}
	return true;
}

StateMachine Game::Loop() {
	for (;;) {
		if ( !Update() )
			return masterStates;
	}
}

bool Game::Update() {
	StateMachine* tracker = new StateMachine();
	bool sent = false;
	//std::string fake;
	each_client {
		auto begin = std::chrono::system_clock::now();
		client->stateChanges = new StateMachine();
		if ( !client->GetUpdatedStates() ) return false;
		std::string message = client->stateChanges->GetMessage();
		if ( message.size() > 0 ) {
			sent = true;
		//	//fake = "on";
		//	//fake += ( char )0;
		//	//fake += ( char )1;
		//	//fake += ( char )0;
			std::cout << "Client Sent: " << std::endl;
			print_string( message );
			auto end = std::chrono::system_clock::now();
			std::cout << "Read Time (s): " << ( ( std::chrono::duration<double> )( end - begin ) ).count() << std::endl;
		} //else {
		//	fake = "";
		//}
		//
	}


	each_client {
		auto begin = std::chrono::system_clock::now();
		masterStates.Interpret( client->stateChanges->GetMessage().c_str(), tracker );
		auto end = std::chrono::system_clock::now();
		if ( sent )
			std::cout << "Interpret Time (s): " << ( ( std::chrono::duration<double> )( end - begin ) ).count() << std::endl;
	}

	//masterStates.Interpret( fake.c_str(), tracker );

	each_client {
		auto begin = std::chrono::system_clock::now();
		client->stateChanges = new StateMachine();
		client->states->Interpret( tracker->GetMessage().c_str(), client->stateChanges );
		std::string message = client->stateChanges->GetMessage();
		//if ( message.size() > 0 ) {
		//	std::cout << "Server Sending: " << std::endl;
		//	print_string( message );
		//}
		if ( !client->SendStates( *client->stateChanges ) ) return false;
		auto end = std::chrono::system_clock::now();
		if ( sent )
			std::cout << "Write Time (s): " << ( ( std::chrono::duration<double> )( end - begin ) ).count() << std::endl;
	}

	//each_client {
		//std::cout << "Updating client " << i++ << "'s states..." << std::endl;
		//client->states.message.push_back( 0 );
		//masterStates.Interpret( client->states.message.c_str() );
		//client->states.message.clear();
	//}

	//std::cout << "Sending updated states..." << std::endl;

	return true;
}
