#include "game.h"


Game::Game() : _numPlayers(4) 
{
	Table t(&_s);
	_t = t;
}

void Game::setNetworks(vector<FFNN_Evo *> & netvec)
{
	_netvec = netvec;
}

void Game::cleanUp()
{
	_t.clear(); // testing only - ideally should do nothing
	_s._tricks1 = 0;
	_s._tricks2 = 0;
	_s._phase = bid;
	_s._round2 = false;// testing only
	_s._trump_flag = false;
	_s._dealer = nextPlayer(_s._dealer);
}

void Game::deal()
{
	_t.deal();
}

void Game::sort()
{
	_t.sort();
}


player_t Game::nextPlayer(player_t p)
{
	if (p == W) return N;
	if (p == N) return E;
	if (p == E) return S;
	return W;
}

void Game::playCard(player_t player, size_t index)
{
	if (player == _s._leader)
	{
		_s._lead = _t.getSuit(player, index);
	}
	_t.playCard(player, index);

}

void Game::discard(player_t player, size_t index)
{

}

void Game::pickUp()
{

}

Player * Game::player(player_t p)
{
	if (p == W) return &_p1;
	if (p == N) return &_p2;
	if (p == E) return &_p3;
	return &_p4;
}


// Start new hand
void Game::newHand()
{
	deal();
	_s._cardup = _t.topCard();
	_s._up = _s._cardup._suit;
	//_s._up = _t.topSuit(); // suit of the tilly
	
}



// ROUND 1 BIDDING
// returns true if bid is made in round 1
bool Game::bid1() 
{
	sort();
	player_t current = nextPlayer(_s._dealer);
	int passCounter = 0;

	while (passCounter < _numPlayers)
	{
		//	cout << "TESTING: " << endl;
		//	cout << "GAME DEALER: " << _s._dealer << endl;
		//	cout << "GAME CURRENT: " << current << endl;
		//_t.showHand(current); cout << endl; 

		if (player(current)->decision() == 1) // decision expects 0 or 1: bid yes/no
		{
			_s._bidder = current; // set bidder
			_s._trump_flag = true; // set trump flag
			_s._trump = _s._up; // set trump

			// Dealer picks up
			_t.pickUp(_s._dealer); // pick up
			_t.sort(_s._dealer); // re-sort dealer's hand


			int discard_index = player(_s._dealer)->decision() - 1; // decision expects 1-6: card to discard
			_t.discard(_s._dealer, discard_index);
	

			return true; // bidding done
		}
		current = nextPlayer(current);
		passCounter++;
	}

	return false;
}



// ROUND 2 BIDDING
// returns true if bid is made in round 2
bool Game::bid2() 
{
	_s._round2 = true;
	player_t current = nextPlayer(_s._dealer);
	int passCounter = 0;
	

	while (passCounter < _numPlayers)
	{
		// NOTE: "going alone" not incorporated
		int suitChosen = player(current)->decision(); // decision expects 0-[number of suits - 1]: pass or bid second suit
		if (suitChosen != 0) 
		{
			_s._bidder = current; // set bidder
			_s._trump_flag = true; // set trump flag
													
			vector<suit_t> suits;
			if (_s._up != diamonds) suits.push_back(diamonds);
			if (_s._up != clubs) suits.push_back(clubs);
			if (_s._up != hearts) suits.push_back(hearts);
			if (_s._up != spades) suits.push_back(spades);

			_s._trump = (suits[suitChosen - 1]);

			_s._round2 = false;
			return true; // bidding done
		}
		current = nextPlayer(current);
		passCounter++;
	}

	

	_s._round2 = false;
	return false;
}

void Game::playHand()
{
	_s._phase = play;
	sort();

	int cardIndex = 0;
	int numPlayed = 0;
	int numTricks = 0;
	bool tookAll = false;
	bool euchred = false;
	player_t current; // tracks current turn
	player_t winner = _s._bidder;

	

	_s._leader = nextPlayer(_s._dealer); // left of dealer starts with lead

	// inform players 


	while (numTricks < 5) // tricks
	{
		if (_numReal> 0) cout << "STARTING ROUND OF PLAY" << endl << endl;
		current = _s._leader; // leader starts play
		while (numPlayed < 4) // each player plays a card
		{
			cardIndex = player(current)->decision() - 1; // decision expects a number between 1 and [handsize]
			playCard(current, cardIndex);
			numPlayed++;
			if (numPlayed < 4)
			{
				current = nextPlayer(current);
			}
			

			// SEND DATA TO PLAYERS
			_p1.recordPlayed(current);
			_p2.recordPlayed(current);
			_p3.recordPlayed(current);
			_p4.recordPlayed(current);
		}
		// compare cards
		player_t trick_winner;
		switch (_t.comparePlayed())
		{
		case 1:
			trick_winner = _s._leader;
			break;
		case 2: 
			trick_winner = nextPlayer(_s._leader);
			break;
		case 3:
			trick_winner = nextPlayer(nextPlayer(_s._leader));
			break;
		case 4:
			trick_winner = current;				
		}

		if (trick_winner == N || trick_winner == S)
		{
			_s._tricks1++;
		}
		else
		{
			_s._tricks2++;
		}

		_s._leader = trick_winner;
		numTricks = _s._tricks1 + _s._tricks2; // increment trick counter
		if (_numReal)
		{
			cout << endl << "Cards Played: "; _t.showPlayed(); cout << endl;
		}
		numPlayed = 0; // reset card counter
		_t.removePlayed(); // clear the board
	}

	if (_s._tricks1 > 2)
	{
		winner = S;
		if (_s._bidder == W || _s._bidder == E)
		{
			euchred = true;
			_s._score1 += 2;
		}
		else if (_s._tricks1 == 5)
		{
			tookAll = true;
			_s._score1 += 2;
		}
		else _s._score1++;
	}
	else
	{
		winner = W;
		if (_s._bidder == N || _s._bidder == S)
		{
			euchred = true;
			_s._score2 += 2;
		}
		else if (_s._tricks2 == 5)
		{
			tookAll = true;
			_s._score2 += 2;
		}
		else _s._score2++;
	}

		if (_numReal > 0) // display for human players
		{
			cout << endl << endl << "ROUND OVER" << endl << endl;
			if (euchred)
			{
				cout << "EUCHRE!" << endl;
				//cout << player(winner)._name << " euchred " << player(nextPlayer(winner))._name << " and receives 2 points" << endl;
			}
			//else if (tookAll) cout << player(winner)._name << " took all the tricks and receives 2 points" << endl;
			//else cout << player(winner)._name << " made the bid and receives 1 point" << endl;


			if (_s._score1 < 10 && _s._score2 < 10) {
				cout << endl << "Score: " << _s._score1 << " to " << _s._score2 << endl << endl;
				cout << "Press ENTER to deal a new hand" << endl;
				system("pause");
			}
		}
}




// MAIN SCRIPT
void Game::newGame(int numReal)
{
	_numReal = numReal;
	string name1 = "West";
	string name2 = "North";
	string name3 = "East";
	string name4 = "South";
	int scoreMax = 10;

	if (numReal > 0)
	{
		cout << "Starting a new game of Euchre." << endl;
		cout << "Press 1 to choose names. Press any other button to start" << endl;
		
		bool specific_names = false;
		string temp;
		std::getline(cin, temp); 
		if (temp == "1")
		{
			if (numReal == 1)
			{
				specific_names = true;
				cout << "Please enter your name (South): " << endl;
				std::getline(cin, name4);
				cout << "Please enter the name for your partner (North): " << endl;
				std::getline(cin, name2);
				cout << "Please enter the name for Team 2, Player 1 (West): " << endl;
				std::getline(cin, name1);
				cout << "Please enter the name for Team 3, Player 2 (East): " << endl;
				std::getline(cin, name3);
			}
			else
			{
				specific_names = true;
				cout << "Please enter the name for Team 1, Player 1 (North) : " << endl;
				std::getline(cin, name2);
				cout << "Please enter the name for Team 1, Player 2 (South): " << endl;
				std::getline(cin, name4);
				cout << "Please enter the name for Team 2, Player 1 (West): " << endl;
				std::getline(cin, name1);
				cout << "Please enter the name for Team 3, Player 2 (East): " << endl;
				std::getline(cin, name3);
			}
		}


		if (numReal == 1)
		{
			Player p1(_netvec[0], &_t, W, name1, name2, name3, name4); // AI
			Player p2(_netvec[1], &_t, N, name2, name3, name4, name1); // AI
			Player p3(_netvec[2], &_t, E, name3, name4, name1, name2); // AI
			Player p4(&_t, S, name4, name1, name2, name3);
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
		}
		else
		{
			Player p1(&_t, W, name1, name2, name3, name4);
			Player p2(&_t, N, name2, name3, name4, name1);
			Player p3(&_t, E, name3, name4, name1, name2);
			Player p4(&_t, S, name4, name1, name2, name3);
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
		}
	




		cout << "Starting a new game of Euchre..." << endl;
		if (specific_names)
		{
			cout << "West: " << name1 << endl;
			cout << "North: " << name2 << endl;
			cout << "East: " << name3 << endl;
			cout << "South: " << name4 << endl;
		}
		cout << "Press ENTER to start the game!" << endl;
		cin.ignore();
		//system("pause");
	}
	else // all AI
	{
		Player p1(_netvec[0], &_t, W, name1, name2, name3, name4); // AI
		Player p2(_netvec[1], &_t, N, name2, name3, name4, name1); // AI
		Player p3(_netvec[2], &_t, E, name3, name4, name1, name2); // AI
		Player p4(_netvec[3], &_t, S, name4, name1, name2, name3); // AI
		_p1 = p1;
		_p2 = p2;
		_p3 = p3;
		_p4 = p4;
	}


	//reset score
	_s._score1 = 0; 
	_s._score2 = 0;
	_s._dealer = W; // NOTE: change to randomly select?

	int skip_counter = 0;

	// MAIN GAME LOOP
	while (_s._score1 < scoreMax && _s._score2 < scoreMax)
	{
		//cout << "Starting New Hand..." << endl;
		if (numReal > 0) cout << "DEALING NEW HAND" << endl;
		newHand();

		// bidding
		bool bidMade = false;
		bidMade = bid1(); // bid round 1
		if (!bidMade) bidMade = bid2(); // bid round 2

		if (bidMade)
		{
			skip_counter = 0;
			playHand();
		}
		else if (numReal > 0) cout << "DEALER PASSES: STARTING NEW HAND" << endl;
		else skip_counter++;
		
		cleanUp();
		if (skip_counter > 10) // everyone passed 10 hands in a row -> end game 0-0
		{
			_s._score1 = 0;
			_s._score2 = 0;
			break;
		}
	} // GAME OVER


	_prev_score_NS = _s._score1;
	_prev_score_WE = _s._score2;
// END OF GAME
	if (numReal > 0)
	//if (true)
	{
		cout << endl << endl << "GAME OVER" << endl;
		cout << "Final score: " << _s._score1 << " to " << _s._score2 << endl;
		if (_s._score1 > 9)
		{
			cout << name2 << " and "  << name4  << " win!" << endl << endl;
		}
		else cout << name1 << " and " << name3 << " win!" << endl << endl;
		cout << "Press ENTER to quit" << endl;
		cin.ignore();
	}
}