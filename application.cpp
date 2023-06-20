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
	if ( !port.open )
		return 0;
	std::cout << "Connected to port " << 1234 << std::endl;

	std::cout << "Waiting for clients" << std::endl;
	Client* client1 = port.WaitForClient();
	std::cout << "Waiting for second client" << std::endl;
	Client* client2 = port.WaitForClient();

	std::cout << "Loading existing params" << std::endl;
	Params clientparms = Params( client1->ip_address + "-" + client2->ip_address + ".prm" );
	if ( clientparms.contents.size() <= 0 )
		clientparms = Params( client2->ip_address + "-" + client1->ip_address + ".prm" );

	if ( clientparms.contents.size() <= 0 )
		std::cout << "Could not find existing parameters" << std::endl;


	// Initialize Game
	Game game = Game( port, clientparms.contents );

	// Connect first client
	game.Connect( client1 );
	std::cout << "Connected to first client" << std::endl;
	game.Connect( client2 );
	std::cout << "Connected to second client" << std::endl;


	// Run game loop
	StateMachine out_states = game.Loop();

	std::cout << "All done!" << std::endl;


	std::string InitialTrialNumber = out_states.GetState( "TrialNumber" );
	std::cout << "Trial Number: " << InitialTrialNumber << std::endl;
	if ( InitialTrialNumber.size() == 0 ) InitialTrialNumber = "\1";
	std::cout << "Saving Trial Number: " << ( int ) *InitialTrialNumber.c_str() - 1 << std::endl;

	Params outparams = Params();
	outparams.AddParam( "Application", "int", "InitialTrialNumber", std::to_string( ( int )*InitialTrialNumber.c_str() - 1 ) );
	std::ofstream egof( client1->ip_address + "-" + client2->ip_address + ".prm" );
	egof << outparams.contents << std::endl;

	std::cout << "All done!" << std::endl;
}

