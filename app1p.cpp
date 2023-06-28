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
#include <fstream>
#include "params.h"

int main() {
	Port port = Port( 1234, 0 );

	std::cout << "Waiting for clients" << std::endl;
	Client* client1 = port.WaitForClient();

	// Initialize Game
	Game game = Game( port, "" );

	// Connect first client
	game.Connect( client1 );

	// Run game loop
	for ( ;; ) {
		if ( !game.ReadClients() )
			break;
		game.Reconcile();

		int zero = 0;
		game.SetState( "on", std::string( ( char* )&zero, sizeof( zero ) ) );


		if ( !game.SendToClients() )
			break;
	}

	std::cout << "All done!" << std::endl;


}

