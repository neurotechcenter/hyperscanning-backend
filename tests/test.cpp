#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"

#define t( x ) { std::cout << __LINE__ << ": " << std::endl; x; std::cout << std::endl; }
#define ts( x ) std::cout << __LINE__ << ": " << std::endl; x; std::cout << std::endl;
#define T( x, ... ) { std::cout << __LINE__ << ": " << std::endl; x( __VA_ARGS__ ); std::cout << std::endl; }
#define Ts( x, ... ) std::cout << __LINE__ << ": " << std::endl; x( __VA_ARGS__ ); std::cout << std::endl;

int main() {

	std::cout << "Beginning Tests" << std::endl;

	//Open Port
	std::cout << "Testing Port" << std::endl;
	t( Port port( 1234, 100 ) );
	t( Port port( 0, 10 ) );
	t( Port port( 10, 0 ) );
	t( Port port( NULL, 20 ) );
	t( Port port( 1234, NULL ) );
	t( Port port( 0, 0 ) );
	t( Port port( NULL, NULL ) );

	//Game Connection
	std::cout << "Testing Game Connection" << std::endl;

	ts( Game game = Game() );
	Ts( game.Connect, NULL, "" );

	//Clients
	std::cout << "Testing Clients" << std::endl;

	t( Client client( 1234, 100 ) );
	t( Client client( 0, 10 ) );
	t( Client client( 10, 0 ) );
	t( Client client( NULL, 20 ) );
	t( Client client( 1234, NULL ) );
	t( Client client( 0, 0 ) );
	t( Client client( NULL, NULL ) );

	t( Client client( 1234, 100 ); client.GetUpdatedStates() );
	t( Client client( 0, 10 ); client.GetUpdatedStates() );
	t( Client client( 10, 0 ); client.GetUpdatedStates() );
	t( Client client( NULL, 20 ); client.GetUpdatedStates() );
	t( Client client( 1234, NULL ); client.GetUpdatedStates() );
	t( Client client( 0, 0 ); client.GetUpdatedStates() );
	t( Client client( NULL, NULL ); client.GetUpdatedStates() );

	std::cout << "Tests Complete" << std::endl;

}

