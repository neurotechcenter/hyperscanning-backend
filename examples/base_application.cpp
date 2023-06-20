#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"
#include "statemachine.h"
#include "params.h"

#include <iostream>


int main() {

	// Load Parameter File
	Params params = Params( "HyperscanningParameters.prm" );

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

