#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include "statemachine.h"

StateMachine::StateMachine() {
}

void StateMachine::Interpret( const char* buffer, StateMachine* tracker = nullptr ) {

	while ( *buffer != '\0' ) {
		//std::cout << "Getting state name" << std::endl;
		std::string name( buffer );
		//bool readonly = false;
		buffer += name.size() + 1;
		//if ( name[ 0 ] == 0 ) {
		//	readonly = true;
		//	name.erase( name.begin() );
		//}
		char size = *buffer++;
		//std::cout << "Interpreting value" << std::endl;
		std::string value( buffer, size );
		buffer += value.size();


		//, readonly );
		if ( !SetState( name, value ) && tracker ){
			tracker->SetState( name, value );
		}
	}
}


std::string StateMachine::GetMessage() {
	std::string out;
	for ( int i = 0; i < StateNames.size(); i++ ) {
		out += StateNames[ i ];
		out += '\0';
		out += StateValues[ i ].size();
		out += StateValues[ i ];

		//std::cout << StateNames[ i ] << ", " << 0 << ", " << StateValues[ i ].size() << ", " << ( int ) StateValues[ i ][ 0 ] << std::endl;
	}
	return out;
}

bool StateMachine::SetState( std::string name, std::string value ) {
	std::string& state = LocateState( name );
	std::string previous = state;
	state.assign( value );
	return previous == state;
}

//void StateMachine::SetState( std::string name, std::string value, bool readonly ) {
//	auto state = LocateState( name );
//	if ( !readonly ) {
//		if ( value != state ) {
//			message += name;
//			message.push_back( '\0' );
//			message.push_back( value.size() );
//			message += value;
//		}
//	}
//
//	state.assign( value );
//}
//
//void StateMachine::SetState( std::string name, char* value, char size ) {
//	auto state = LocateState( name );
//	if ( value != state ) {
//		message += name;
//		message.push_back( '\0' );
//		message.push_back( size );
//		message += std::string( value, size );
//	}
//
//	state.assign( value );
//}

std::string& StateMachine::LocateState( std::string name ) {
	auto it = std::find( StateNames.begin(), StateNames.end(), name );
	if ( it != StateNames.end() ) {
		return StateValues[ it - StateNames.begin() ];
	}
	StateNames.push_back( name );
	StateValues.push_back( std::string( 1, '\0' ) );
	return StateValues[ StateNames.size() - 1 ];
}

const char* StateMachine::GetState( std::string name ) {
	return LocateState( name ).c_str();
}

StateMachine::~StateMachine() {
}

