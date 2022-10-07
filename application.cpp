#include "statemachine.h"
#include "port.h"
#include "client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

int main() {
	Port port( 1234, 100 );
	std::cout << "Connected to port " << 1234 << std::endl;

	std::cout << "Waiting for clients" << std::endl;

	Client* client1 = port.WaitForClient();
	std::cout << "Connected to first client" << std::endl;
	std::cout << "Waiting for second client" << std::endl;
	Client* client2 = port.WaitForClient();
	std::cout << "Connected to second client" << std::endl;

	//Greeting Messages
	std::cout << "Sending Greeting" << std::endl;
	std::string greeting( "Hello from the server!" );
	std::cout << "Greeting: " << greeting.c_str() << ", "  << greeting.size() << std::endl;
	send( client1->connection, greeting.c_str(), greeting.size(), 0 );
	send( client2->connection, greeting.c_str(), greeting.size(), 0 );

	std::string param_file( "HyperscanningParameters.prm" );
	std::fstream file( param_file );
	std::string contents( std::istreambuf_iterator<char>( file ), ( std::istreambuf_iterator<char>() ) );
	send( client1->connection, contents.c_str(), contents.size(), 0 );
	send( client2->connection, contents.c_str(), contents.size(), 0 );

	client1->states.SetState( "ClientNumber", ( char ) 0 );
	client2->states.SetState( "ClientNumber", ( char ) 1 );

	char n = 0;

	auto time = std::chrono::system_clock::now();
	auto interval = std::chrono::milliseconds( 1 );

	for (;;) {
		time = std::chrono::system_clock::now();

		std::cout << "Awaiting Updated States" << std::endl;
		client1->GetUpdatedStates();
		client2->GetUpdatedStates();

		std::cout << "Recieved States" << std::endl;
		std::cout << "Current States: " << std::endl;
		std::cout << "	Client 1: " << std::endl;
		std::cout << "		Color: " << (int) *client1->states.GetState( "Color" ) << std::endl;
		std::cout << "	Client 2: " << std::endl;
		std::cout << "		Color: " << (int) *client2->states.GetState( "Color" ) << std::endl;

		std::cout << "Sending Updated States" << std::endl;
		client1->SendStates( client2->states );
		client2->SendStates( client1->states );
		client1->states.message.clear();
		client2->states.message.clear();
		n++;

		//std::this_thread::sleep_until( time + interval );
	}

	std::cout << "All done!" << std::endl;
}
