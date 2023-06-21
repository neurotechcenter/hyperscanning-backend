#include "game.h"
#include "statemachine.h"
#include "port.h"
#include "client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <signal.h>


void Game::ConnectClient( Client* client ) {
	size_t size = params.size();
	send( client->connection, &size, sizeof( params.size() ), 0 );
	send( client->connection, params.c_str(), params.size(), 0 );
	client->ClientNumber = clients.size();
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

void Game::ValidateStates( Client* client ) {
	char* mBuffer = ( char* ) calloc( 1028, 1 );
	std::cout << "Waiting for client to send shared states" << std::endl;
	recv( client->connection, mBuffer, 1028, 0 );
	std::cout << "Recieved client shared states" << std::endl;

	char m = 0;
	if ( sharedStates.size() ) {
		if ( std::string( mBuffer ) == sharedStates ) {
			m = 1;
			std::cout << "Shared states match" << std::endl;
		}
		else {
			m = 2;
			std::cout << "Shared states don't match" << std::endl;
		}
	} else {
		sharedStates = std::string( mBuffer );
		std::cout << "First client" << std::endl;
	}
	std::cout << "Sending client validation result." << std::endl;
	send( client->connection, &m, 1, 0 );
	std::cout << "Sent validation result" << std::endl;
}

bool Game::Connect( Client* client ) {
	if ( client ) {
//		for ( auto c : clients ) {
//			if ( c->Matches( client ) )
//				return false;
//		}

		std::cout << "Connecting Client" << std::endl;
		ConnectClient( client );
		std::cout << "Validating states" << std::endl;
		clients.push_back( client );
		ValidateStates( client );
	} else {
		std::cout << "Client does not exist" << std::endl;
		return false;
	}
	return true;
}

void Game::AddClient( Client* client ) {
	clients.push_back( client );
}

StateMachine Game::Loop() {
	for (;;) {
		if ( !Update() )
			return masterStates;
	}
}

bool Game::ReadClients() {
	if ( tracker ) delete tracker;
	tracker = new StateMachine();
	each_client {
		if ( client->stateChanges ) delete client->stateChanges;
		client->stateChanges = new StateMachine();
		if ( !client->GetUpdatedStates() ) return false;
		std::string message = client->stateChanges->GetMessage();
		if ( message.size() > 0 ) {
			std::cout << "Client Sent: " << std::endl;
			print_string( message );
		}
	}
	return true;
}

void Game::Reconcile( ) {
	each_client {
		masterStates.Interpret( client->stateChanges->GetMessage().c_str(), tracker );
	}
}

bool Game::SendToClients( ) {
	each_client {
		if ( !SendToClient( client ) ) return false;
	}
	return true;
}

bool Game::SendToClient( Client* client ) {
		if ( client->stateChanges ) delete client->stateChanges;
		client->stateChanges = new StateMachine();
		client->states->Interpret( tracker->GetMessage().c_str(), client->stateChanges );
		std::string message = client->stateChanges->GetMessage();
		if ( !client->SendStates( *client->stateChanges ) ) return false;
		return true;
}

bool Game::Update() {
	StateMachine* tracker = new StateMachine();

	if ( !ReadClients() )
		return false;

	Reconcile();

	if ( !SendToClients() )
		return false;

	return true;

}

void Game::SetState( std::string name, std::string value ) {
	if ( !masterStates.SetState( name, value ) && tracker ) {
		tracker->SetState( name, value );
	}
}

const char* Game::GetState( std::string name ) {
	return masterStates.GetState( name );
}
