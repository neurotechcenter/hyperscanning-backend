#include "statemachine.h"
#include "port.h"
#include "client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

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
	client->states.message.clear();
}

class Game {
	public:
	std::vector<Client*> clients;
	StateMachine masterStates = StateMachine();

#define each_client for ( auto client : clients )

	void Connect( Client* client ) {
		clients.push_back( client );
	}

	void Update() {
//		std::cout << "Getting updated states..." << std::endl;
		each_client client->GetUpdatedStates();

		masterStates.message.clear();
		each_client {
			std::cout << "Message: " << std::endl;
			std::cout << client->states.message << std::endl;
			client->states.message.push_back( 0 );
			masterStates.Interpret( client->states.message.c_str() );
			client->states.message.clear();
		}

//		std::cout << "Sending updated states..." << std::endl;
		each_client client->SendStates( masterStates );

		std::cout << "Color: " << ( int ) *masterStates.GetState( "Color" ) << std::endl;
	}
};

int main() {
	std::string param_file( "HyperscanningParameters.prm" );
	std::fstream file( param_file );
	std::string contents( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
	std::cout << "Initial Size: " << contents.size() << std::endl;
	contents.push_back( 0 );
	//contents += std::string( 1025 - ( contents.size() % 1025 ), 0 );

	std::cout << "First zero: " << contents.find_first_of( ( char )0 ) << std::endl;

	std::cout << "Size: " << contents.size() << std::endl;

	Game game = Game();

	Port port( 1234, 100 );
	std::cout << "Connected to port " << 1234 << std::endl;

	std::cout << "Waiting for clients" << std::endl;

	Client* client1 = port.WaitForClient();
	connect_client( client1, contents, 0 );
	game.Connect( client1 );
	std::cout << "Connected to first client" << std::endl;

	std::cout << "Waiting for second client" << std::endl;
	Client* client2 = port.WaitForClient();
	connect_client( client2, contents, 1 );
	game.Connect( client2 );
	std::cout << "Connected to second client" << std::endl;

	auto time = std::chrono::system_clock::now();
	auto interval = std::chrono::milliseconds( 1 );


	for (;;) {
		time = std::chrono::system_clock::now();

		//Client* new_client = port.CheckForClient();
		//if ( new_client )
		//	game.Connect( new_client );

		game.Update();
	}

	std::cout << "All done!" << std::endl;
}

