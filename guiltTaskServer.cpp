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
#include <cstdio>
#include "params.h"
#include <vector>
#include <sstream>
#include <string>

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

int main() {

	// Load Parameter File
	Params params = Params( "GuiltTaskSharedParameters.prm" );

	// Load previous parameters

	Port port( 1234, 100 );
	if ( !port.open )
		return 0;
	std::cout << "Connected to port " << 1234 << std::endl;

	std::cout << "Waiting for clients" << std::endl;
	Client* client1 = port.WaitForClient();
	std::cout << "Waiting for second client" << std::endl;
	Client* client2 = port.WaitForClient();

	// Hyperscanning Application Base Compatible Version
	std::string version = "1.0";

	// Check that versions are compatible
	if ( client1->version != client2->version ) {
		std::cout << "Client Versions Do Not Match" << std::endl;
		std::cout << "Client 1 Version: " << client1->version << std::endl;
		std::cout << "Client 2 Version: " << client2->version << std::endl;
		std::cout << "Please use clients with matching versions to ensure bug free usage" << std::endl;
		return -1;
	}

	if ( client1->version != version ) {
		std::cout << "Client versions are incompatible with this version of the backend" << std::endl;
		std::cout << "Client Version: " << client1->version << std::endl;
		std::cout << "Backend Version: " << version << std::endl;
		std::cout << "Please use clients and backend with compatible versions to ensure bug free usage" << std::endl;
		return -1;
	}

	if ( client1->sid != client2->sid ) {
		std::cout << "Client  IDs do not match" << std::endl;
		std::cout << "Client 1 sID: " << client1->sid << std::endl;
		std::cout << "Client 2 sID: " << client2->sid << std::endl;
		std::cout << "Please use matching session IDs" << std::endl;
		return -1;
	}

	if ( client1->sid.size() == 0 && client2->sid.size() == 0 ) {
		std::cout << "Session ID not provided by clients" << std::endl;
		return -1;
	}

	std::cout << "Loading existing params" << std::endl;
	//bool swaped = false;
	//Params clientparms = Params( client1->id + "-" + client2->id + ".prm" );
	//if ( clientparms.contents.size() <= 0 ) {
	//	clientparms = Params( client2->id + "-" + client1->id + ".prm" );
	//	if ( clientparms.contents.size() ) swaped = true;
	//	
	//}

	std::cout << "Client1 SID: " << client1->sid << std::endl;
	Params clientparms = Params( client1->sid + ".prm" );

	std::string TrialSequence;
	int trials;

	if ( clientparms.contents.size() > 0 ) {
		std::cout << "Found existing game" << std::endl;
		params.contents += clientparms.contents;

		TrialSequence = clientparms.GetParam( "TrialSequence" )->line;
		trials = clientparms.GetParam( "TrialSequence" )->length;
		std::cout << "trials(trial number): " << trials << std::endl;
	}
	else {
		// Generate Random Sequence

		std::cout << "Did not find existing game" << std::endl;
		std::cout << "Generating new values..." << std::endl;
        //randomly pick trial type sequence
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(1, 2);
        int trial_type_random = distrib(gen);
        std::cout << "trial sequence index is " << trial_type_random << std::endl;

		Param* trialsq = params.GetParam( "TrialSequence" + std::to_string(trial_type_random));
		std::cout << "Got trail sequence" << std::endl;
		std::cout << "trial sequence name : " << trialsq->name << std::endl;
		std::cout << "trial sequence line : " << trialsq->line << std::endl;

		std::string my_target = trialsq->name + "=";
		size_t my_begin = trialsq->line.find(my_target);
		my_begin += my_target.length() + 1;
		std::string subs = trialsq->line.substr(my_begin);

		//split by space
		char my_space = ' ';
		std::vector<std::string> segments = split(subs, my_space);
		if(segments.size() == 0){
			std::cout << trialsq->name + " is empty" << std::endl;
		}

		int my_number = std::stoi(segments[0]);
		std::vector<int> order;

		for (size_t i = 1; i < my_number + 1 && i < segments.size(); i++) {
			order.push_back(std::stoi(segments[i]));
    	}

		std::cout << "New " + trialsq->name + " is below and "  << "Size:  "  << order.size() << std::endl; 
    	for(int ele : order){       
        	std::cout << ele << std :: endl;
    	}

		std::cout << "trials(trial number): " << my_number << std::endl;
		trials = my_number;

		// std::random_device rd;
		// auto rng = std::default_random_engine( rd() );
		// std::shuffle( std::begin( order ), std::end( order ), rng );

		std::cout << "First: " << order[ 0 ] << std::endl;

		TrialSequence = "\nApplication:Experiment intlist TrialSequence= ";
		TrialSequence += std::to_string( order.size() );
		TrialSequence += " ";
		for ( int i = 0; i < order.size(); i++ ) {
			TrialSequence += std::to_string( order[ i ] );
			TrialSequence += " ";
		}
		TrialSequence += "% % % //Random trial type sequence";

		std::cout << TrialSequence << std::endl;

		params.AddParam( TrialSequence );

		params.AddParam( "Application int InitialTrialNumber= 0 % % % // trial number" );
	}

	params.contents.push_back( 0 );

	std::cout << "Size: " << params.contents.size();

	if ( clientparms.contents.size() <= 0 )
		std::cout << "Could not find existing parameters" << std::endl;


	// Initialize Game
	Game game = Game( port, params.contents );

	// Connect first client
	std::ifstream indatfile( client1->sid + ".dat" );
	std::string firstclient;
	indatfile >> firstclient;
	if ( firstclient == client1->id ) {
		game.Connect( client1 );
		std::cout << "Connected to first client: " << client1->id << std::endl;
		game.Connect( client2 );
		std::cout << "Connected to second client: " << client2->id  << std::endl;
	} else {
		game.Connect( client2 );
		std::cout << "Connected to first client: " << client2->id << std::endl;
		game.Connect( client1 );
		std::cout << "Connected to second client: " << client1->id  << std::endl;

		//Swap Clients
		auto c = client2;
		client2 = client1;
		client1 = c;
	}



	// Run game loop
	StateMachine out_states = game.Loop();

	std::cout << "All done!" << std::endl;


	std::string InitialTrialNumber = out_states.GetState( "TrialNumber" );
	std::cout << "Trial Number: " << InitialTrialNumber << std::endl;
	if ( InitialTrialNumber.size() == 0 ) InitialTrialNumber = "\1";
	std::cout << "Saving Trial Number: " << ( int ) *InitialTrialNumber.c_str() - 1 << std::endl;

	if ( ( int ) *InitialTrialNumber.c_str() < trials ) {

		Params outparams = Params();
		outparams.AddParam( "Application", "int", "InitialTrialNumber", std::to_string( ( int )*InitialTrialNumber.c_str() - 1 ) );
		outparams.AddParam( TrialSequence );
		//if ( !swaped ) {
		//	std::ofstream egof( client1->id + "-" + client2->id + ".prm" );
		//	egof << outparams.contents << std::endl;
		//}
		//else {
		//	std::ofstream egof( client2->id + "-" + client1->id + ".prm" );
		//	egof << outparams.contents << std::endl;
		//}
		std::ofstream egof( client1->sid + ".prm" );
		egof << outparams.contents << std::endl;
		egof.close();

		std::ofstream datfile( client1->sid + ".dat" );
		datfile << client1->id << std::endl;
		datfile << client2->id << std::endl;
		datfile.close();
	} else {
		std::cout << "Trials finished, so not saving game" << std::endl;
		std::remove( ( client1->sid + ".prm" ).c_str() );
		std::remove( ( client1->sid + ".dat" ).c_str() );
	}


	std::cout << "All done!" << std::endl;
}

