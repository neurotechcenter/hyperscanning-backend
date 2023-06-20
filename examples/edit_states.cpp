#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"
#include "statemachine.h"
#include "params.h"

#include <iostream>

#define statewise( x ) std::string( ( char* ) &x, sizeof( x ) )

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

	int x = 0;

	// Run custom game loop
	for ( ;; ) {
		if ( !game.ReadClients() ) break; // if client disconnects, end the loop
		game.Reconcile();

		// Update state
		x++;
		game.SetState( "x", statewise( x ) ); // statewise defined on line 10

		if ( !game.SendToClients() ) break; // if client disconnects, end the loop
	}

	std::cout << "All done!" << std::endl;
}

