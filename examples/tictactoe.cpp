#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"
#include "statemachine.h"
#include "params.h"

#include <iostream>


int main() {
	// Open Port
	Port port( 1234, 100 );
	if ( !port.open )
		return 0;
	std::cout << "Connected to port " << 1234 << std::endl;

	// Load Parameter File
	Params params = Params( "TicTacToe.prm" );

	std::cout << "Waiting for clients" << std::endl;
	Client* client1 = port.WaitForClient();
	std::cout << "Waiting for second client" << std::endl;
	Client* client2 = port.WaitForClient();

	bool swaped = false;
	Params clientparms = Params( client1->id + "-" + client2->id + ".prm" );
	if ( clientparms.contents.size() <= 0 ) {
		clientparms = Params( client2->id + "-" + client1->id + ".prm" );
		if ( clientparms.contents.size() ) swaped = true;
		
	}

	params.contents += clientparms.contents;

	if ( clientparms.contents.size() <= 0 )
		std::cout << "Could not find existing parameters" << std::endl;


	// Initialize Game
	Game game = Game( port, params.contents );

	if ( !swaped ) {
		game.Connect( client1 );
		std::cout << "Connected to first client" << std::endl;
		game.Connect( client2 );
		std::cout << "Connected to second client" << std::endl;
	} else {
		game.Connect( client2 );
		std::cout << "Connected to first client" << std::endl;
		game.Connect( client1 );
		std::cout << "Connected to second client" << std::endl;
	}

	std::cout << "Waiting for clients" << std::endl;

	// Run game loop
	StateMachine outstates = game.Loop();

	std::string board = outstates.GetState( "grid" );
	std::string turn = outstates.GetState( "turn" );
	Params outparams = Params();
	uint64_t val = 0;
	memcpy( &val, board.c_str(), board.size() );
	outparams.AddParam( "Application", "int", "grid", std::to_string( val ) );
	val = 0;
	memcpy( &val, turn.c_str(), turn.size() );
	outparams.AddParam( "Application", "int", "turn", std::to_string( val ) );

	if ( !swaped ) {
		std::ofstream egof( client1->id + "-" + client2->id + ".prm" );
		egof << outparams.contents << std::endl;
	}
	else {
		std::ofstream egof( client2->id + "-" + client1->id + ".prm" );
		egof << outparams.contents << std::endl;
	}

	std::cout << "All done!" << std::endl;
}

