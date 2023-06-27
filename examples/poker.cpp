#include "statemachine.h"
#include "game.h"
#include "port.h"
#include "client.h"
#include "statemachine.h"
#include "params.h"

#include <iostream>
#include <random>
#include <vector>
#include <cstdlib>

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

	auto client1 = port.WaitForClient();
	auto client2 = port.WaitForClient();
	//auto client3 = port.WaitForClient();

	game.Connect( client1 );
	game.Connect( client2 );
	//game.Connect( client3 );

	std::cout << "Connected to first client" << std::endl;
	std::cout << "Waiting for second client" << std::endl;

	// Connect second client

	std::cout << "Connected to second client" << std::endl;

	int phase = 0;
	// 0: Dealing
	// 1: Bet
	// 2: Turn over
	// 3: Win test
	// 4: Win output

	std::vector<int> order = std::vector<int>( 52 );

	int n = 0;

	int players = 2;

	std::vector<int> playerHands = std::vector<int>( 2 * players );
	std::vector<int> bets = std::vector<int>( players );
	std::vector<int> money = std::vector<int>( players, 100 );
	std::vector<bool> folded = std::vector<bool>( players, false );
	int nFolded = 0;

	std::vector<int> river = std::vector<int>( 5 );

	std::vector<int> handRank = std::vector<int>( players );
	std::vector<int> cardRank = std::vector<int>( players );

	int nRiver = 0;
	int r = 55;

	int add = 0;
	int aow = 5;
	bool rego = true;
	bool regoa = false;
	int regoman = 0;
	int regoaman = 0;

	bool setwinner = false;

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

				river = std::vector<int>( 5 );
				for ( int i = 0; i < 5; i++ ) {
					game.SetState( "river" + std::to_string( i ), statewise( r ) );
				}
				bets = std::vector<int>( players );
				folded = std::vector<bool>( players, false );
				nRiver = 0;
				add = 0;
				aow = 4;
				rego = true;
				regoa = false;
				regoman = 0;
				regoaman = 0;
				setwinner = false;
				nFolded = 0;

				for ( int i = 0; i < players; i++ ) {
					handRank[ i ] = 10;
					int d0 = order[ n++ ];
					int d1 = order[ n++ ];
					playerHands[ 2 * i ] = d0;
					playerHands[ 2 * i + 1 ] = d1;

				}


				bets[ abs( ( regoman - 2 ) % players ) ] = 2;
				money[ abs( ( regoman - 2 ) % players ) ] -= 2;
				bets[ abs( ( regoman - 1 ) % players ) ] = 4;
				money[ abs( ( regoman - 1 ) % players ) ] -= 4;

				phase++;
				add = 0;
				aow = 4;
				game.SetState( "confirmed", statewise( add ) );
				break;
			}

			// Bets
			case 1: {
				int turn = ( int )  *(  game.GetState( "turn" ) );
				int bet = ( int ) *(  game.GetState( "bet" ) );
				int confirmed = ( int ) * (  game.GetState( "confirmed" ) );

				game.SetState( "pbet", statewise( bets[ turn ] ) );

				if ( players - nFolded == 1 ) {
					phase = 3;
					std::cout << "win by default" << std::endl;
				}

				if ( confirmed ) {

					if ( bet == 0 ) {
						folded[ turn ] = true;
						nFolded++;
						std::cout << "Player " << turn << " Folded" << std::endl;

						if ( players - nFolded == 1 ) {
							phase = 3;
							std::cout << "Win by default" << std::endl;
						}
					}
					else
						money[ turn ] -= bet - bets[ turn ];

					bets[ turn ] = bet;
					if ( bet > aow ) {
						if ( !rego ) {
							std::cout << "Rego" << std::endl;
							rego = true;
							regoa = false;
							regoman = abs( ( turn ) % players );
						} else {
							std::cout << "Rego again" << std::endl;
							regoa = true;
							rego = false;
							regoaman = abs( ( turn ) % players );
						}

						aow = bet;
					}


					std::cout << "Player " << turn << " bet " << bet << std::endl;
					int msgp = turn + 1;
					game.SetState( "msgp", statewise( msgp ) );
					game.SetState( "msg", statewise( bets[ turn ] ) );


					turn = ( turn + 1 ) % players;
					while ( folded[ turn ] ) {
						std::cout << "Skipping player " << turn << " because they folded" << std::endl;
						if ( ( turn == regoman && !( regoa ) ) || ( turn == regoaman && !rego ) ) {
							std::cout << "Finished betting" << std::endl;
							phase++;
						}
						turn = ( turn + 1 ) % players;
					}

					std::cout << "It is now player " << turn << "'s turn" << std::endl;

					std::cout << turn << ", " << regoman << ", " << regoa << ", " << regoaman << ", " << rego << std::endl;
					if ( ( turn == regoman && !( regoa ) ) || ( turn == regoaman && !rego ) ) {
						std::cout << "Finished betting" << std::endl;
						phase++;
					}

					game.SetState( "turn", statewise( ( turn ) ) );
					confirmed = 0;
					game.SetState( "confirmed", statewise( confirmed ) );



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
					if ( folded[ i ] ) {
						handRank[ i ] = 10;
						cardRank[ i ] = 0;
						continue;
					}

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
							if ( hand[ l + q ] - hand[ l ] != q && ! ( hand[ l ] % 13 == 0 && hand[ l + q ] % 13 == 12 && hand[ l + q ] - hand[ l ] == 12 ) )
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

						card = playerHands[ i * 2 + 1 ];
						number = 0;
					}

					if ( maxNumber == 4 ) {
						handRank[ i ] = 2;
						cardRank[ i ] = fourCard;
						continue;
					}

					if ( n2s >= 1 && n3s >= 1 ) {
						handRank[ i ] = 3;
						cardRank[ i ] = rank3s[ 0 ] * 13 + rank2s[ 0 ];
						continue;
					}

					// Flush
					std::vector<int> ranks = std::vector<int>( 5 );
					for ( int l = 0; l < 3; l++ ) {
						int errs = 0;
						for ( int q = 0; q < 7; q++ ) {
							if ( hand[ l + q ] / 13 !=  hand[ l ] / 13 )
								errs++;
							else
								ranks[ q - errs ] = hand[ l + q ] % 13;
							if ( errs > 2 - l )
								break;
							if ( q == 6 - l ) {
								handRank[ i ] = 4;
								std::sort( ranks.begin(), ranks.end() );
								int p = 1;
								cardRank[ i ] = 0;
								for ( int z = 0; z < 5; z++ ) {
									cardRank[ i ] += ranks[ z ] * p;
									p *= 13;
								}
							}
						}
					}

					if ( handRank[ i ] == 4 )
						continue;



					// Straight

					std::vector<int> aHand = std::vector<int>( 7 );
					for ( int l = 0; l < 7; l++ ) {
						aHand[ l ] = hand[ l ] % 13;
					}
					std::sort( aHand.begin(), aHand.end() );
					
					for ( int l = 0; l < 3; l++ ) {
						int errs = 0;
						for ( int q = 0; q < 5; q++ ) {
							if ( aHand[ l + q ] != aHand[ l ] + q && ! ( aHand[ l + q ] % 13 == 12 && aHand[ l] == 0 ) )
								break;
							if ( q == 4 ) {
								handRank[ i ] = 5;
								cardRank[ i ] = ( aHand[ l ] % 13 == 0 && aHand[ l + q ] % 13 == 12 ) ? 4 : aHand[ l + q ];
							}
						}
					}

					if ( handRank[ i ] == 5 )
						continue;


					// Three of a kind
					if ( n3s >= 1 ) {
						handRank[ i ] = 6;
						cardRank[ i ] = rank3s[ 0 ] * 13 * 13;
						std::vector<int> maxi;
						for ( int l = 0; l < 7; l++ ) {
							if ( hand[ l ] % 13 != rank3s[ 0 ] ) {
								maxi.push_back( hand[ l ] % 13 );
							}
						}
						std::sort( maxi.begin(), maxi.end() );
						cardRank[ i ] += maxi[ 3 ] * 13 + maxi[ 2 ];
						continue;
					}

					// Two pair
					if ( n2s > 1 ) {
						handRank[ i ] = 7;
						cardRank[ i ] = ( *std::max_element( rank2s.begin(), rank2s.end() ) ) * 13 * 13 + ( *std::min_element( rank2s.begin(), rank2s.end() ) ) * 13;
						int max = -1;
						for ( int l = 0; l < 5; l++ ) {
							if ( river[ l ] % 13 != rank2s[ 0 ] && river[ l ] % 13 != rank2s[ 1 ] && river[ l ] % 13 > max )
								max = river[ l ] % 13;
						}

						cardRank[ i ] += max;

						continue;
					}

					// Pair
					if ( n2s == 1 ) {
						handRank[ i ] = 8;
						cardRank[ i ] = rank2s[ 0 ] * 13 * 13 * 13;
						std::vector<int> maxi;
						for ( int l = 0; l < 7; l++ ) {
							if ( hand[ l ] % 13 != rank2s[ 0 ] ) {
								maxi.push_back( hand[ l ] % 13 );
							}
						}
						std::sort( maxi.begin(), maxi.end() );
						cardRank[ i ] += maxi[ 4 ] * 13 * 13 + maxi[ 3 ] * 13 + maxi[ 2 ];
						continue;
					}

					handRank[ i ] = 9;
					cardRank[ i ] = aHand[ 6 ] * 13 * 13 * 13 * 13 + aHand[ 5 ] * 13 * 13 * 13 + aHand[ 4 ] * 13 * 13 + aHand[ 3 ] * 13 + aHand[ 2 ];

				}

				phase++;

				break;
			}

			case 4: {
				int minScore = 10;
				int maxBugger = 0;
				int minPlayer = NULL;
				bool tie = false;
				int tiers = 1;
				std::vector<bool> ties = std::vector<bool>( players );
				for ( int i = 0; i < players; i++ ) {
					if ( folded[ i ] ) {
						std::cout << "Player " << i << " folded" << std::endl;
						continue;
					}
					std::cout << "Player " << i << " got a hand of rank " << handRank[ i ] << " with a card of rank " << cardRank[ i ] << std::endl;
					if ( handRank[ i ] < minScore ) {
						minScore = handRank[ i ];
						maxBugger = cardRank[ i ];
						minPlayer = i;
					} else if ( handRank[ i ] == minScore ) {
						if ( cardRank[ i ] > maxBugger ) {
							tie = false;
							minPlayer = i;
							maxBugger = cardRank[ i ];
						}
						else if ( cardRank[ i ] == maxBugger ) {
							tie = true;
							ties[ minPlayer ] = true;
							ties[ i ] = true;
							tiers++;
						}
					}
				}

				int pot = 0;
				for ( int i = 0; i < players; i++ ) {
					pot += bets[ i ];
				}

				if ( tie ) {
					std::cout << "Tie" << std::endl;
					game.SetState( "tie", statewise( tie ) );
					for ( auto tier : ties ) {
						if ( tier ) {
							std::cout << "Player " << tier << " tied" << std::endl;
							money[ tier ] += pot / tiers;
						}
					}
				} else {
					std::cout << "Player " << minPlayer << " won with a hand of rank " << handRank[ minPlayer ] << std::endl;
					game.SetState( "winner", statewise( minPlayer ) );
					money[ minPlayer ] += pot;
				}
				phase++;
			}
			case 5: {
				if ( ( int ) ( * game.GetState( "confirmed" ) ) )
					phase = 0;

				break;
			}

			default:
				std::cerr << "Ruh-roh" << std::endl;
		}

		game.SetState( "phase", statewise( phase ) );
		game.SetState( "aow", statewise( aow ) );

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

