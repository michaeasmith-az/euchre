#include "player.h"

Player::Player(Table * t, player_t side, const string name, const string name2, const string name3, const string name4): 
	_t(t), _me(side), _name(name), _name2(name2), _name3(name3), _name4(name4), _real(true)  {
	//cout << "CONSTRUCTING NEW PLAYER" << endl; // TESTING

	// Construct a map of side to player name
	player_t next1 = nextPlayer(_me);
	player_t next2 = nextPlayer(next1);
	player_t next3 = nextPlayer(next2);
	_players.insert({ _me, name });
	_players.insert({ next1, name2 });
	_players.insert({ next2, name3 });
	_players.insert({ next3, name4 });

	_ai = NULL;
}

Player::Player(FFNN_Evo * network, Table * t, player_t side, const string name, const string name2, const string name3, const string name4) :
	_t(t), _me(side), _name(name), _name2(name2), _name3(name3), _name4(name4), _real(false) {
	//cout << "CONSTRUCTING NEW PLAYER" << endl; // TESTING

	// Construct a map of side to player name
	player_t next1 = nextPlayer(_me);
	player_t next2 = nextPlayer(next1);
	player_t next3 = nextPlayer(next2);
	_players.insert({ _me, _name });
	_players.insert({ next1, _name2 });
	_players.insert({ next2, _name3 });
	_players.insert({ next3, _name4 });

	_ai = new Player_AI(_t->_state, _me, _t->hand(_me), &_t->_inPlay, network);
}

// copy constructor
Player::Player(const Player &old) : 
	_t(old._t), _me(old._me), _name(old._name), _name2(old._name2), _name3(old._name3), _name4(old._name4), _real(old._real)
{
	// Construct a map of side to player name
	player_t next1 = nextPlayer(_me);
	player_t next2 = nextPlayer(next1);
	player_t next3 = nextPlayer(next2);
	_players.insert({ _me, _name });
	_players.insert({ next1, _name2 });
	_players.insert({ next2, _name3 });
	_players.insert({ next3, _name4 });

	_ai = new Player_AI(*old._ai);
}

Player& Player::operator=(const Player & old)
{
	_t = old._t;
	_me = old._me;
	_name = old._name;
	_name2 = old._name2;
	_name3 = old._name3;
	_name4 = old._name4;
	_real = old._real;

	// Construct a map of side to player name
	player_t next1 = nextPlayer(_me);
	player_t next2 = nextPlayer(next1);
	player_t next3 = nextPlayer(next2);
	_players.insert({ _me, _name });
	_players.insert({ next1, _name2 });
	_players.insert({ next2, _name3 });
	_players.insert({ next3, _name4 });

	if (old._ai != NULL) {
		_ai = new Player_AI(*old._ai);
	}
	

	return *this;
}

Player::~Player()
{
	delete _ai;
}


void Player::showP(player_t p)
{
	cout << _players[p];
}

void Player::showS(suit_t s)
{
	switch (s)
	{
	case 1:
		cout << "Diamonds";
		break;
	case 2: 
		cout << "Clubs";
		break;
	case 3: 
		cout << "Hearts";
		break;
	case 4:
		cout << "Spades";
	}
}

player_t Player::nextPlayer(player_t p)
{
	if (p == W) return N;
	if (p == N) return E;
	if (p == E) return S;
	return W;
}

int Player::processInput(int first_choice, int last_choice, string input)
{
	for (int i = first_choice; i < last_choice + 1; i++)
	{
		if (input == std::to_string(i)) return i;
	}
	return -1;
}

bool Player::followSuit(int index)
{
	if (_me == s._leader)
		return true;
	suit_t lead = s._lead;
	if (_t->checkSuit(_me, lead)) // if suit is in hand
	{
		//cout << endl << "Suit is in hand!" << endl << endl;
		if (_t->getSuit(_me, index) != lead) // if selected card does not share suit
		{
			//cout << "Lead suit: " << lead << endl;
			//cout << "Suit: " << _t->getSuit(_me, index) << endl;
			//cout << "Index: " << index << endl;
			//cout << "DOES NOT FOLLOW SUIT!" << endl;
			return false; // selected card does not follow suit
		}
		else
		{
			//cout << "Lead suit: " << lead << endl;
			//cout << "Suit: " << _t->getSuit(_me, index) << endl;
			//cout << "Index: " << index << endl;
		}
	}

	return true; // selected card follows suit OR no card in hand follows suit

}

void Player::header()
{
	// (mostly) GENERAL HEADER

	cout << endl << endl << endl << "****************************" << endl;
	cout << "Score: " << s._score1 << " to " << s._score2 << endl;
	cout << _players[N] << " and " << _players[S] << " vs " << _players[W] << " and " << _players[E] << endl << endl;
	

//		cout << "Phase: ";	
	if (s._phase == bid)
	{
		cout << "Phase: ";
		if (!s._round2) cout << " Bid, round 1" << endl;
		else cout << " Bid, round 2" << endl;
	}
	//else cout << "Play" << endl;
	

	cout << "Dealer: "; showP(s._dealer); cout << endl;

	//cout << endl << "YOUR TURN, " << _name << endl << endl;
	cout << endl << endl;

	visualHeader();
	specificHeader(); // situational

	// Show hand at end:
	cout << endl  << "Hand: "; _t->showHand(_me); cout << endl;
}

void Player::visualHeader()
{
	
	// show board position
	if (s._phase == bid)
	{
		player_t dealer = s._dealer;
		if (dealer == N) cout << "  D  " << endl;
		else cout << "  X  " << endl;
		if (dealer == W) cout << "D";
		else cout << "X";
		if (dealer == E) cout << "   D" << endl;
		else cout << "   X" << endl;
		if (dealer == S) cout << "  D  " << endl;
		else cout << "  X  " << endl;
		cout << endl;
	}
	else
	{
		player_t leader = s._leader;
		if (leader == N) cout << "  L  " << endl;
		else cout << "  X  " << endl;
		if (leader == W) cout << "L";
		else cout << "X";
		if (leader == E) cout << "   L" << endl;
		else cout << "   X" << endl;
		if (leader == S) cout << "  L  " << endl;
		else cout << "  X  " << endl;
		cout << endl;
	}
}

void Player::specificHeader()
{
	if (s._phase == bid && !s._trump_flag) // Bid
	{
		bidHeader();
	}
	else if (s._phase == bid) // Dealer Discard
	{
		if (_me == s._bidder) cout << "You declared trump." << endl;
		else cout << _players[s._bidder] << " declared trump." << endl;
		cout << endl << "TRUMP: "; showS(s._trump); cout << endl << endl;
		cout << "Pick a card to discard" << endl;
	}
	else // Play phase
	{
		playHeader();
	}
}

void Player::bidHeader()
{
	if (!s._round2) // round 1
	{
		cout << "Card turned up: ";
		_t->showTop();
		cout << endl << endl;
	}
	else // round 2
	{
		cout << "Suit turned down: ";
		showS(s._up);
		cout << endl << endl;
	}
	// Display all players who have already passed:
	player_t temp_p = nextPlayer(s._dealer);
	while (temp_p != _me) 
	{
		cout << _players[temp_p] << ": pass" << endl;
		temp_p = nextPlayer(temp_p);
	}
	cout << "Your bid" << endl;
}

void Player::playHeader()
{
	if (_me == s._bidder) cout << "You declared trump." << endl;
	else cout << _players[s._bidder] << " declared trump." << endl;

	cout << endl << "TRUMP: "; showS(s._trump); cout << endl << endl;
	cout << "Tricks: " << s._tricks1 << " to " << s._tricks2 << endl << endl;
	if (_me == s._leader)
	{
		cout << "You have the lead. Choose a card to play." << endl;
	}
	else
	{
		cout << "Cards played: " << endl;
		_t->showPlayed();
	}
}

int Player::bid1()
{
	string bid1;
	bool error = false;
	//bool accepted = false;

	while (1)
	{
		header();
		if (error) cout << "ERROR - please try again." << endl;
		cout << endl << "Enter 0 or 1" << endl;
		cout << "0: Pass, 1: ";
		if (_me == s._dealer) cout << "Pick it up" << endl;
		else cout << "Order it up" << endl;
		std::getline(cin, bid1);
		if (bid1 == "0") return 0;
		if (bid1 == "1") return 1;
		error = true;
	}
}

int Player::discard()
{
	string input;
	int index;
	bool error = false;

	while (1)
	{
		header();
		if (error) cout << endl << "ERROR - please try again" << endl;
		cout << endl << "Enter 1-6 to select a card from your hand" << endl;
		std::getline(cin, input);
		index = processInput(1, 6, input);
		if (index != -1)
		{
			Card c = _t->getCard(_me, index - 1);
			if (!_real)_ai->recordDiscard(c);
			return index;
		}
		error = true;
		
	}
	return 1;
}

int Player::bid2()
{
	string bid2;
	string input;
	bool error = false;

	// vector of suits still remaining for bidding
	vector<suit_t> suits;
	if (s._up != diamonds) suits.push_back(diamonds);
	if (s._up != clubs) suits.push_back(clubs);
	if (s._up != hearts) suits.push_back(hearts);
	if (s._up != spades) suits.push_back(spades);

	while (1)
	{
		header();
		if (error) cout << "ERROR - please try again." << endl;
		cout << endl << "Enter 0-3 to pass or bid a suit." << endl;
		cout << "0: Pass,";
		cout << " 1: Bid "; showS(suits[0]); 
		cout << " 2: Bid "; showS(suits[1]);
		cout << " 3: Bid "; showS(suits[2]);
		cout << endl; 

		std::getline(cin, input);
		int index = processInput(0, 3, input);

		//cout << "YOU CHOSE: "; showS(suits[index - 1]); cout << endl; ///////// TESTING

		if (index != -1) return index;
		error = true;		
	}
	return 0;
}

int Player::playCard()
{
	int size = _t->handSize(_me);
	string input;
	int index;
	bool error = false;
	bool error_suit = false;

	while (1)
	{
		header();
		if (error) cout << "ERROR - please try again." << endl;
		if (error_suit)
		{
			cout << "You must follow suit." << endl;
			error_suit = false;
		}
		cout << "Enter 1-" << size << " to select a card from your hand to play:" << endl;
		std::getline(cin, input);
		index = processInput(1, size, input);
		if (index != -1)
		{
			if (followSuit(index - 1))
				return index;
			else
				error_suit = true;
		}
		error = true;
	}

	return 0;
}

bool Player::followSuit_AI(int index)
{
	if (_me == s._leader) return true;
	suit_t lead = s._lead;
	if (_t->checkSuit(_me, lead)) // if suit is in hand
	{
		if (_t->getSuit(_me, index) != lead) // if selected card does not share suit
		{
			return false; // selected card does not follow suit
		}
	}
	return true; // selected card follows suit OR no card in hand follows suit

}

int Player::bid1_AI()
{
	int num_possible_trump = 1;
	if (s._round2) num_possible_trump = 3;
	int index = _ai->bidDecide();
	if (index < 0 || index > num_possible_trump)
	{
		cout << "ERROR IN BID: CHOSEN INDEX: " << index << " OUT OF RANGE. SELECTING RANDOM BID." << endl;
		index = rand() % (num_possible_trump + 1);
	}
	return index;
}

int Player::discard_AI()
{
	int size = 6;
	int index = _ai->discardDecide(); // expects value between 1-6
	if (index < 1 || index > size)
	{
		cout << "ERROR IN DISCARD: CHOSEN INDEX: " << index << " OUT OF RANGE. SELECTING RANDOM CARD." << endl;
		index = rand() % size + 1;
	}

	return index;
}

int Player::bid2_AI()
{
	return bid1_AI();
}

int Player::playCard_AI()
{
	int size = _t->handSize(_me);
	int index = 0;

	while (1)
	{

		index = _ai->playDecide();
		if (index < 1 || index > size)
		{
			cout << "ERROR: CHOSEN INDEX: " << index << " OUT OF RANGE. SELECTING RANDOM CARD." << endl;
			index = rand() % size + 1;
			return index;
		}
		if (followSuit_AI(index - 1))
		{
			return index;
		}
		else
		{
			cout << "ERROR: AI picked an illegal card." << endl;
		}
	}

	return 0;
}

void Player::recordPlayed(player_t current)
{
	if (!_real)
	{
		Card c = _t->getPlayed();
		_ai->processCard(c, current);
	}
}

int Player::decision() 
{
	s = *(_t->_state);
																	//	cout << "PLAYER DEALER: " << s._dealer << endl;
																	//	cout << "PLAYER CURRENT: " <<  _me << endl;
	int choice;

	if (_real)
	{
		if (s._phase == bid && !s._round2 && !s._trump_flag) choice = bid1(); // Bid first round:  0 or 1
		else if (s._phase == bid && s._trump_flag) choice = discard();
		else if (s._round2) choice = bid2();
		else choice = playCard();
	}
	else
	{
		if (s._phase == bid && !s._round2 && !s._trump_flag) choice = bid1_AI(); // Bid first round:  0 or 1
		else if (s._phase == bid && s._trump_flag) choice = discard_AI();
		else if (s._round2) choice = bid2_AI();
		else choice = playCard_AI();
	}

	return choice;
}