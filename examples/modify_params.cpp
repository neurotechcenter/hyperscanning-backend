#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"
#include "statemachine.h"
#include "params.h"

#include <algorithm>
#include <iostream>
#include <random>


int main() {

	// Load Parameter File
	Params params = Params( "HyperscanningParameters.prm" );


	//
	// Generate Random Sequence
	//

	std::string stimuliSequence;

	// Get number of stimuli
	Param* stimmat = params.GetParam( "StimuliMatrix" );


	// Get vector with numbers from 1 to n, where n is the number of stimuli
	std::vector<int> order = std::vector<int>( stimmat->width );
	for ( int i = 0; i < stimmat->width; i++ )
		order[ i ] = i;


	// Randomize order
	std::random_device rd;
	auto rng = std::default_random_engine( rd() );
	std::shuffle( std::begin( order ), std::end( order ), rng );

	// Create string with randomized numbers
	for ( int i = 0; i < stimmat->width; i++ ) {
		stimuliSequence += std::to_string( order[ i ] );
		stimuliSequence += " ";
	}

	// Add the parameter
	params.AddParam( "Application:Sequence", "intlist", "StimuliSequence", stimuliSequence );

	params.contents.push_back( 0 );

	// Open Port
	Port port( 1234, 100 );
	if ( !port.open )
		return 0;
	std::cout << "Connected to port " << 1234 << std::endl;

	// Initialize Game
	Game game = Game( port, params.contents );

	std::cout << "Waiting for clients" << std::endl;

	// Connect first client
	game.Connect( port.WaitForClient() );

	std::cout << "Connected to first client" << std::endl;
	std::cout << "Waiting for second client" << std::endl;

	// Connect second client
	game.Connect( port.WaitForClient() );

	std::cout << "Connected to second client" << std::endl;

	// Run game loop
	game.Loop();

	std::cout << "All done!" << std::endl;
}

