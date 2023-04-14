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
	std::string param_file( "HyperscanningParameters.prm" );
	std::fstream file( param_file );
	std::string contents( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
	std::cout << "Initial Size: " << contents.size() << std::endl;

	// Load previous parameters
	std::fstream egf( "ExistingGame.txt" );
	std::string InitialTrialNumber;
	std::string egc( std::istreambuf_iterator<char>( egf ), ( std::istreambuf_iterator<char>() ) );
	std::string stimuliSequence = "Application:Sequence intlist StimuliSequence= ";

	if ( egc.size() > 0 ) {
		contents += egc;

		size_t ssl = egc.find( stimuliSequence );
		size_t sse = egc.find( '\n', ssl );
		stimuliSequence = egc.substr( ssl, sse - ssl );
	}
	else {
		// Generate Random Sequence
		size_t param = contents.find( "StimuliMatrix" );
		param = contents.find( "}", param );
		size_t size = contents.find( " ", param ) - param;
		std::string lenstr = contents.substr( param, size );
		int len = std::stoi( lenstr );

		std::vector<int> order = std::vector<int>( len );
		for ( int i = 0; i < len; i++ )
			order[ i ] = i;

		std::random_device rd;
		auto rng = std::default_random_engine( rd() );
		std::shuffle( std::begin( order ), std::end( order ), rng );

		std::string stimuliSequence = "Application:Sequence list StimuliSequence= ";
		stimuliSequence += std::to_string( len );
		stimuliSequence += " ";
		for ( int i = 0; i < len; i++ ) {
			stimuliSequence += std::to_string( order[ i ] );
			stimuliSequence += " ";
		}
		stimuliSequence += " % % % //Random Stimuli Sequence";
		contents += stimuliSequence;
		contents.push_back( '\n' );
		contents += "Application int InitialTrialNumber= 0 % % % // trial number";
		contents.push_back( '\n' );
	}

	contents.push_back( 0 );

	std::cout << "First zero: " << contents.find_first_of( ( char )0 ) << std::endl;

	std::cout << "Size: " << contents.size() << std::endl;

	Port port( 1234, 100 );
	std::cout << "Connected to port " << 1234 << std::endl;

	std::vector<Game> games = std::vector<Game>();
	games.push_back( Game( port, contents ) );

	std::cout << "Waiting for clients" << std::endl;

	Client* client1 = port.WaitForClient();
	//connect_client( client1, contents, 0 );
	games[ 0 ].Connect( client1 );
	std::cout << "Connected to first client" << std::endl;

	std::cout << "Waiting for second client" << std::endl;
	Client* client2 = port.WaitForClient();
	//connect_client( client2, contents, 1 );
	games[ 0 ].Connect( client2 );
	std::cout << "Connected to second client" << std::endl;

	auto time = std::chrono::system_clock::now();
	auto interval = std::chrono::milliseconds( 1 );

	int nClients = 2;
	int nGames = 1;

	//for (;;) {
	//	time = std::chrono::system_clock::now();

	//	Client* new_client = port.CheckForClient();
	//	//if ( new_client && !check_existing_client( new_client, games ) ) {
	//	//	nClients++;
	//	//	if ( nClients % 2 == 0 ) {
	//	//		games.push_back( Game() );
	//	//		nGames++;
	//	//	}
	//	//	games[ nGames - 1 ].Connect( new_client, contents );
	//	//}

	//}
	StateMachine out_states = games[ 0 ].Loop();

	std::ofstream egof( "ExistingGame.prm" );
	InitialTrialNumber = out_states.GetState( "trialNum" );
	egof << "Application int InitialTrialNumber= " << InitialTrialNumber << " % % % // trial number" << std::endl;
	egof << stimuliSequence << std::endl;
	std::cout << "All done!" << std::endl;
}

