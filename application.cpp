#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"
#include "statemachine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <random>
#include "params.h"


bool check_existing_client( Client* new_client, std::vector<Game> games ) {
	for ( auto game : games ) {
		for ( auto client: game.clients ) {
			if ( client->ip_address == new_client->ip_address && client->port == new_client->port ) {
				client = new_client;
				StateMachine s = StateMachine();
				for ( int i = 0; i < game.masterStates.StateNames.size(); i++ ) {
					s.SetState( game.masterStates.StateNames[ i ], game.masterStates.StateValues[ i ] );
				}
				client->SendStates( s );
				return true;
			}
		}
	}
	return false;
}

int main() {

	// Load Parameter File
	Params params = Params( "HyperscanningParameters.prm" );

	// Load previous parameters
	Params existing = Params( "ExistingGame.prm" );

	std::string stimuliSequence;

	if ( existing.contents.size() > 0 ) {
		params.contents += existing.contents;

		stimuliSequence = existing.GetParam( "StimuliSequence" )->line;
	}
	else {
		// Generate Random Sequence

		Param* stimmat = params.GetParam( "StimuliMatrix" );

		std::vector<int> order = std::vector<int>( stimmat->width );
		for ( int i = 0; i < stimmat->width; i++ )
			order[ i ] = i;

		std::random_device rd;
		auto rng = std::default_random_engine( rd() );
		std::shuffle( std::begin( order ), std::end( order ), rng );

		stimuliSequence = "\nApplication:Sequence intlist StimuliSequence= ";
		stimuliSequence += std::to_string( stimmat->width );
		stimuliSequence += " ";
		for ( int i = 0; i < stimmat->width; i++ ) {
			stimuliSequence += std::to_string( order[ i ] );
			stimuliSequence += " ";
		}
		stimuliSequence += "% % % //Random Stimuli Sequence";

		params.AddParam( stimuliSequence );

		params.AddParam( "Application int InitialTrialNumber= 0 % % % // trial number" );
	}

	params.contents.push_back( 0 );

	Port port( 1234, 100 );
	std::cout << "Connected to port " << 1234 << std::endl;

	std::vector<Game> games = std::vector<Game>();
	games.push_back( Game( port, params.contents ) );

	std::cout << "Waiting for clients" << std::endl;

	Client* client1 = port.WaitForClient();
	games[ 0 ].Connect( client1 );

	std::cout << "Connected to first client" << std::endl;
	std::cout << "Waiting for second client" << std::endl;

	while ( !games[ 0 ].Connect( port.WaitForClient() ) );

	std::cout << "Connected to second client" << std::endl;

	auto time = std::chrono::system_clock::now();
	auto interval = std::chrono::milliseconds( 1 );

	int nClients = 2;
	int nGames = 1;

	StateMachine out_states = games[ 0 ].Loop();

	std::ofstream egof( "ExistingGame.prm" );
	std::string InitialTrialNumber = out_states.GetState( "TrialNumber" );
	std::cout << "Trial Number: " << InitialTrialNumber << std::endl;
	if ( InitialTrialNumber.size() == 0 ) InitialTrialNumber = "\0";
	std::cout << "Saving Trial Number: " << ( int ) *InitialTrialNumber.c_str() << std::endl;
	egof << "Application int InitialTrialNumber= " << ( int )*InitialTrialNumber.c_str() << " % % % // trial number" << std::endl;
	egof << stimuliSequence << std::endl;
	std::cout << "All done!" << std::endl;
}

