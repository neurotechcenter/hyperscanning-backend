#include "statemachine.h"
#include "port.h"
#include "client.h"
#include <iostream>

int main() {
	StateMachine states;
	Port port( 9999, 100 );

	Client client1 = port.WaitForClient();

	for (;;) {
		if ( client1.GetUpdatedStates( states ) ) {
			std::cout << "bob: " << *( ( int* )states.GetState( "bob" ) )  << std::endl;
		}

		//states.SetState( "tom", "abcd" );

		port.SendStates( states );
	}
}
