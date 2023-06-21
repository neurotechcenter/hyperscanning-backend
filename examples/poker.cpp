#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"
#include "statemachine.h"
#include "params.h"

#include <iostream>
#include <random>
#include <vector>

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
	//game.Connect( port.WaitForClient() );

	std::cout << "Connected to second client" << std::endl;

	int phase = 0;
	// 0: Dealing
	// 1: Bet
	// 2: Turn over
	// 3: Win test
	// 4: Win output

	std::vector<int> order = std::vector<int>( 52 );

	int n = 0;

	int players = 1;

	std::vector<int> playerHands = std::vector<int>( 2 * players );
	std::vector<int> bets = std::vector<int>( players );
	std::vector<int> money = std::vector<int>( players );

	std::vector<int> river = std::vector<int>( 5 );

	std::vector<int> handRank = std::vector<int>( players );
	std::vector<int> cardRank = std::vector<int>( players );

	int nRiver = 0;

	int add = 0;

	// Run game loop
	for ( ;; ) {

		if ( !game.ReadClients() ) break; // if client disconnects, end the loop
		game.Reconcile();

		// Update state

		switch ( phase ) {
			// Initial Deal
			case 0: {
				for ( int i = 0; i < order.size(); i++ )
					order[ i ] = i;

				// Randomize order
				std::random_device rd;
				auto rng = std::default_random_engine( rd() );
				std::shuffle( std::begin( order ), std::end( order ), rng );

				for ( int i = 0; i < players; i++ ) {
					int d0 = order[ n++ ];
					int d1 = order[ n++ ];
					playerHands[ 2 * i ] = d0;
					playerHands[ 2 * i + 1 ] = d1;
				}

				phase++;
				add = 0;
				game.SetState( "turn", statewise( add ) );
				game.SetState( "confirmed", statewise( add ) );
				break;
			}

			// Bets
			case 1: {
				int turn = ( int )  *(  game.GetState( "turn" ) );
				int bet = ( int ) *(  game.GetState( "bet" ) );
				int confirmed = ( int ) * (  game.GetState( "confirmed" ) );

				if ( confirmed ) {

					bets[ turn ] = bet;

					std::cout << "Player " << turn << " bet " << bet << std::endl;

					game.SetState( "turn", statewise( ( ++turn ) ) );
					confirmed = 0;
					game.SetState( "confirmed", statewise( confirmed ) );

					if ( turn == players ) {
						phase++;
					}

				}

				break;
			}

			// Deal to river
			case 2: {
				switch ( nRiver ) {
					case 0:
						add = 3;
						break;

					case 3:
						add = 1;
						break;

					case 4:
						add = 1;
						break;

					case 5:
						add = 0;
						break;

					default:
						std::cerr << "Ruh-roh-river" << std::endl;
				}

				for ( int i = nRiver; i < nRiver + add; i++ ) {
					river[ i ] = order[ n++ ];
					game.SetState( "river" + std::to_string( i ), statewise( river[ i ] ) );
				}

				nRiver += add;

				if ( add ) {
					phase--;
					int turn = 0;
					game.SetState( "turn", statewise( turn ) );
				}
				else
					phase++;

				break;
			}

			// Win conditions
			case 3: {
				for ( int i = 0; i < players; i++ ) {
					std::vector<int> hand = std::vector<int>( 7 );
					for ( int l = 0; l < 5; l++ )
						hand[ l ] = ( river[ l ] );
					hand[ 5 ] = playerHands[ i * 2 ];
					hand[ 6 ] = playerHands[ i * 2 + 1 ];

					std::sort( hand.begin(), hand.end() );

					// Straight Flush
					for ( int l = 0; l < 3; l++ ) {
						int errs = 0;
						for ( int q = 0; q < 7 - l; q++ ) {
							if ( hand[ l + q ] - hand[ l ] != q )
								errs++;
							if ( errs > 2 - l )
								break;
							if ( q == 6 - l ) {
								handRank[ i ] = 1;
								cardRank[ i ] = ( hand[ l ] % 13 );
							}
						}
					}

					if ( handRank[ i ] == 1 ) continue;

					// Four of a kind, full house, three of a kind, two pair, or pair
					int card = playerHands[ i * 2 ];
					int number = 0;
					int maxNumber = 0;
					int n2s = 0;
					std::vector<int> rank2s = std::vector<int>( 3 );
					int n3s = 0;
					std::vector<int> rank3s = std::vector<int>( 2 );
					int fourCard;
					std::vector<int> illegals = std::vector<int>( 7 );

					std::cout << "Duplicates check" << std::endl;
					for ( int l = 0; l < 2; l++ ) {
						std::cout << "Card: " << card << ", " << card % 13 << std::endl;
						for ( int q = 0; q < 7; q++ ) {
							if ( hand[ q ] % 13 == card % 13 ) {
								number++;
								std::cout << "Matched with: " << hand[ q ] << ", " << hand[ q ] % 13 << ", for " << number << " matches" << std::endl;
							} else {
								std::cout << "Not matched with: " << hand[ q ] << ", " << hand[ q ] % 13 << std::endl;
							}
						}

						if ( number > maxNumber )
							maxNumber = number;
						if ( number == 2 ) {
							rank2s[ n2s++ ] = ( hand[ l ] % 13 );
						}
						if ( number == 3 )
							rank3s[ n3s++ ] = ( hand[ l ] % 13 );
						if ( number == 4 )
							fourCard = ( hand[ l ] % 13 );

						card = hand[ i * 2 + 1 ];
						number = 0;
					}

					if ( maxNumber == 4 ) {
						handRank[ i ] = 2;
						cardRank[ i ] = fourCard;
						continue;
					}

					if ( n2s >= 1 && n3s >= 1 ) {
						handRank[ i ] = 3;
						cardRank[ i ] = rank3s[ 0 ];
						continue;
					}

					// Flush
					for ( int l = 0; l < 3; l++ ) {
						int errs = 0;
						for ( int q = 0; q < 7; q++ ) {
							if ( hand[ l + q ] / 13 !=  hand[ l ] / 13 )
								errs++;
							if ( errs > 2 - l )
								break;
							if ( q == 6 - l ) {
								handRank[ i ] = 4;
								cardRank[ i ] = ( hand[ l ] % 13 );
							}
						}
					}

					if ( handRank[ i ] == 4 )
						continue;



					// Straight
					for ( int l = 0; l < 3; l++ ) {
						int errs = 0;
						for ( int q = 0; q < 7; q++ ) {
							if ( hand[ l + q ] % 13 !=  hand[ l ] % 13 )
								errs++;
							if ( errs > 2 - l )
								break;
							if ( q == 6 - l ) {
								handRank[ i ] = 5;
								cardRank[ i ] = ( hand[ l ] % 13 );
							}
						}
					}

					if ( handRank[ i ] == 5 )
						continue;


					// Three of a kind
					if ( n3s >= 1 ) {
						handRank[ i ] = 6;
						cardRank[ i ] = rank3s[ 0 ];
						continue;
					}

					// Two pair
					if ( n2s > 1 ) {
						handRank[ i ] = 7;
						cardRank[ i ] = *std::max_element( rank2s.begin(), rank2s.end() );
						continue;
					}

					// Pair
					if ( n2s == 1 ) {
						handRank[ i ] = 8;
						int oMax = 0;
						for ( int l = 0; l < 7; l++ ) {
							if ( hand[ l ] % 13 != rank2s[ 0 ] ) {
								if ( hand[ l ] % 13 > oMax )
									oMax = hand[ l ] % 13;
							}
						}
						cardRank[ i ] = rank2s[ 0 ] * 13 + oMax;
						continue;
					}

					handRank[ i ] = 9;
					int oMax = 0;
					for ( int l = 0; l < 7; l++ ) {
						if ( hand[ l ] % 13 > oMax )
							oMax = hand[ l ] % 13;
					}
					cardRank[ i ] = oMax;

				}

				phase++;

				break;
			}

			case 4: {
				int minScore = 10;
				int maxBugger = 0;
				int minPlayer = NULL;
				bool tie = false;
				std::vector<bool> ties = std::vector<bool>( players );
				for ( int i = 0; i < players; i++ ) {
					std::cout << "Player " << i << " got a hand of rank " << handRank[ i ] << std::endl;
					if ( handRank[ i ] < minScore ) {
						minScore = handRank[ i ];
						maxBugger = cardRank[ i ];
						minPlayer = i;
					}

					if ( handRank[ i ] == minScore ) {
						if ( cardRank[ i ] > maxBugger ) {
							minPlayer = i;
							maxBugger = cardRank[ i ];
						}
						if ( cardRank[ i ] == maxBugger ) {
							tie = true;
							ties[ minPlayer ] = true;
							ties[ i ] = true;
						}
					}
				}

				if ( tie ) {
					std::cout << "Tie" << std::endl;
					for ( auto tier : ties ) {
						std::cout << "Player " << tier << "tied" << std::endl;
						game.SetState( "tie", statewise( tie ) );
					}
				} else {
					std::cout << "Player " << minPlayer << " won with a hand of rank " << handRank[ minPlayer ] << std::endl;
					game.SetState( "winner", statewise( minPlayer ) );
				}

				//if ( ( int )  *(  game.GetState( "confirmed" ) ) )
				//	phase = 0;

				break;
			}

			default:
				std::cerr << "Ruh-roh" << std::endl;
		}

		game.SetState( "phase", statewise( phase ) );

		bool connected = true;
		for ( int i = 0; i < players; i++ ) {
			game.tracker->SetState( "deal00", statewise( playerHands[ 2 * i ] ) );
			game.tracker->SetState( "deal01", statewise( playerHands[ 2 * i + 1 ] ) );
			game.tracker->SetState( "money", statewise( money[ i ] ) );
			if ( !game.SendToClient( game.clients[ i ]  ) ) connected = false;
		}
		if ( !connected ) break; // if client disconnects, end the loop
	}

	std::cout << "All done!" << std::endl;
}

