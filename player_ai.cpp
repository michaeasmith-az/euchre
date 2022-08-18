#include "player_ai.h"

Player_AI::Player_AI(State * s, player_t me, Deck * hand, Deck * played, FFNN_Evo * network) : _s(s), _me(me), _hand(hand), _played(played), _num_cards_played(0), _network(network)
{
	_left_opp = nextPlayer(_me);
	_partner = nextPlayer(_left_opp);
	_right_opp = nextPlayer(_partner);

	// per hand data
	_me_dealer = false;
	_me_bidder = false;
	_partner_bidder = false;
	_discarded = false;
	_round_processed = false;

	// metadata
	_num_cards_played = 0;
	_cards_played.clear();
	_suits_led.resize(4);
	for (auto it = _suits_led.begin(); it != _suits_led.end(); it++)
	{
		*it = false;
	}
	vector<int> temp(4, 6);
	_remaining_in_suit = temp;

	_trump_remaining.resize(7);
	for (auto it = _trump_remaining.begin(); it != _trump_remaining.end(); it++)
	{
		*it = true;
	}

	_values_remaining.resize(4);
}


// ****Helper functions -----------------------------------------------------------------------

player_t Player_AI::nextPlayer(player_t p)
{
	if (p == W) return N;
	if (p == N) return E;
	if (p == E) return S;
	return W;
}

int Player_AI::suit_to_num(suit_t s) // output: 0-3
{
	return s - 1;
}

int Player_AI::card_to_num(Card c) // output: 1-7
{
	if (c._suit == _trump)
	{
		if (c._value == jack) return 7; // right bauer
		if (c._value == ace) return 5; // ace of trump
		if (c._value > 11) return c._value - 9; // queen or king of trump (see deck.h for enum)
		return c._value - 8; // 9 or 10 of trump
	}
	if (c._suit == _next) 
	{
		if (c._value == jack) return 6; // left bauer
		if (c._value == ace) return 5; // next ace
		if (c._value > 11) return c._value - 9; // queen or king of next
		return c._value - 8; // 9 or 10 of next
	}
	if (c._value == ace) return 6; // off-suit ace (not next)
	return c._value - 8; // off suit king or below (not next)
}

int Player_AI::card_to_num_test(Card c, suit_t trump)
{
	suit_t next;
	if (trump == diamonds) next = hearts;
	else if (trump == clubs) next = spades;
	else if (trump == hearts) next = diamonds;
	else next = clubs;

	if (c._suit == trump)
	{
		if (c._value == jack) return 7; // right bauer
		if (c._value == ace) return 5; // ace of trump
		if (c._value > 11) return c._value - 9; // queen or king of trump (see deck.h for enum)
		return c._value - 8; // 9 or 10 of trump
	}
	if (c._suit == next)
	{
		if (c._value == jack) return 6; // left bauer
		if (c._value == ace) return 5; // next ace
		if (c._value > 11) return c._value - 9; // queen or king of next
		return c._value - 8; // 9 or 10 of next
	}
	if (c._value == ace) return 6; // off-suit ace (not next)
	return c._value - 8; // off suit king or below (not next)
}

suit_t Player_AI::realSuit(Card c)
{
	suit_t suit = c._suit;
	if (suit == _next && c._value == jack) suit = _trump; // check if left bauer

	return suit;
}

suit_t Player_AI::realSuit_test(Card c, suit_t trump)
{

	suit_t next = clubs;
	if (trump == diamonds) next = hearts;
	else if (trump == clubs) next = spades;
	else if (trump == hearts) next = diamonds;

	suit_t suit = c._suit;
	if (suit == next && c._value == jack) suit = trump; // check if left bauer

	return suit;
}

void Player_AI::displayChoice(string message, int index)
{
	cout << endl << message << index << endl;
	_hand->print_deck(1);
}

int Player_AI::playCompare(Card a, Card b) // returns 1 if a greater, 2 if b greater
{

	// check for bauers
	if (a._value == jack && a._suit == _trump) return 1; // a is right bauer
	if (b._value == jack && b._suit == _trump) return 2; // b is right bauer
	if (a._value == jack && a._suit == _next) return 1; // a is left bauer 
	if (b._value == jack && b._suit == _next) return 2; // b is left bauer

// no bauer present -->

	// compare suits
	if (a._suit != b._suit) // different suit
	{
		if (a._suit == _trump) return 1;
		if (b._suit == _trump) return 2;
		if (a._suit == _lead_suit) return 1;
		if (b._suit == _lead_suit) return 2;
		return 0; // each non-trump, non-lead suit
	}

//same suit -->

	// compare values
	if (a._value == ace) return 1;
	if (b._value == ace) return 2;
	if (a._value > b._value) return 1;
	return 2;
}

void Player_AI::comparePlayed() // assumes it is AI's turn
{
	Card a = _played->getCard(0); // greatest card so far
	Card b;
	int temp_winner = 1; // position of winner of the trick so far (1 = leader)
	player_t temp_player = _s->_leader; // winner of the trick so far
	player_t temp_winning_player = temp_player;
	

	for (size_t i = 1; i < _played->size(); i++)
	{
		temp_player = nextPlayer(temp_player);
		b = _played->getCard(i);
		if (playCompare(a, b) == 2) // if the second is greater
		{
			a = b; // new greatest
			temp_winner = i + 1; // new winner
			temp_winning_player = temp_player;
		}
	}

	_winner = temp_winner;
	_top_player = temp_winning_player;
	_top_card = a;
}

void Player_AI::resetValuesRemaining()
{
	for (size_t i = 0; i < 4; i++)
	{
		// determine number of cards in suit
		if (i == suit_to_num(_trump))
		{
			_values_remaining[i].resize(7);
		}
		else if (i == suit_to_num(_next))
		{
			_values_remaining[i].resize(5);
		}
		else
		{
			_values_remaining[i].resize(6);
		}

		// initialize
		for (size_t j = 0; j < _values_remaining[i].size(); j++)
		{
			_values_remaining[i][j] = true;
		}
	}
}

int Player_AI::getTeamScore()
{
	if (_me == N || _me == S) return _s->_score1;
	return _s->_score2;
}

int Player_AI::getOppScore()
{
	if (_me == N || _me == S) return _s->_score2;
	return _s->_score1;
}

// makeInput
// creates an input vector for a neural network using the game state and hand
//
// uses the following classes and member variables:
//			state: _dealer, _up, _cardup
//			pllayer_ai: _right_opp, _left_opp, _partner, _hand
// uses the folling player_ai member functions:
//			getTeamScore, getOppScore, card_to_num_test, realSuit
vector<double> Player_AI::makeInput( bool round2, suit_t trump_in_question)
{
	/*
	Input for Euchre bidding neural network
	num_elements : description
	1 : team score
	1 : opponent score
	4 : player position
	2 : first or second round
	2 : relation betweeen trump suit in question and suit turned down (meaningless in round 1, so it is set to 0 then)
	6 : card turned up
	24 : cards in hand
	Total inputs: 40

	Encodings:
	Score:
	Integer encoding -- 0-9 for team score and opponent score
	Player position:
	Binary encoding -- 1st through 4th elements: 1st through 4th positions clockwise from left of dealer
	e.g. 0 1 0 0 = across from dealer
	Round:
	binary encoding -- 1st: round 1, 2nd: round 2
	1 0 = round 1
	0 1 = round 2
	Relation between trump suit and turned up suit:
	binary encoding -- 1st element: opposite color as turned down suit, 2nd element: same color ("next")
	1 0 = same color ("next")
	0 1 = opposite color
	0 0 = round 1
	Card turned up:
	binary encoding -- 1st-6th elements: card value (only 6 possible since it must be trump, but cannot be left bauer)
	e.g. 0 0 0 0 0 1 -- jack of trump (highest)
	e.g. 0 1 0 0 0 0 -- ten of trump
	Card in hand:
	binary encoding -- 1st-6th elements: offsuit 2, 7th-12th: offsuit 1, 13th-17th: next, 18th-24th: trump
	e.g. 0 0 0 0 0 1, 0 1 0 0 0 0, 0 0 1 0 0, 0 0 0 1 0 0 1
	= (assuming hearts are trump) ace of spades, ten of clubs, queen of diamonds, king of hearts, jack of hearts
	*/
	suit_t suit_up = _s->_up;
	suit_t trump = suit_up;
	if (round2) trump = trump_in_question;

	suit_t next;
	suit_t opposite1;
	suit_t opposite2;

	if (trump == diamonds)
	{
		next = hearts;
		opposite1 = clubs;
		opposite2 = spades;
	}
	else if (trump == clubs)
	{
		next = spades;
		opposite1 = diamonds;
		opposite2 = hearts;
	}
	else if (trump == hearts)
	{
		next = diamonds;
		opposite1 = clubs;
		opposite2 = spades;
	}
	else // trump == spades
	{
		next = clubs;
		opposite1 = diamonds;
		opposite2 = hearts;
	}

	vector<double> temp(40, 0.0);

	//score - temp[0-1]
	temp[0] = (double)getTeamScore();
	temp[1] = (double)getOppScore();

	// position - temp[2-5]
	player_t dealer = _s->_dealer;
	if (_right_opp == dealer) temp[2] = 1.0;
	else if (_partner == dealer) temp[3] = 1.0;
	else if (_left_opp == dealer) temp[4] = 1.0;
	else temp[5] = 1.0;

	// round - temp[6-7]
	if (!round2) temp[6] = 1.0;
	else temp[7] = 1.0;

	// relation betwen trump in question and suit turned down - temp[8-9]
	if (round2)
	{
		if (suit_up == diamonds)
		{
			if (trump_in_question == hearts) temp[8] = 1.0; // same color
			else temp[9] = 1.0;
		}
		else if (suit_up == clubs)
		{
			if (trump_in_question == spades) temp[8] = 1.0;
			else temp[9] = 1.0;
		}
		else if (suit_up == hearts)
		{
			if (trump_in_question == diamonds) temp[8] = 1.0;
			else temp[9] = 1.0;
		}
		else // spades
		{
			if (trump_in_question == clubs) temp[8] = 1.0;
			else temp[9] = 1.0;
		}
	}

	// card turned up - temp[10-15]
	int val = card_to_num_test(_s->_cardup, suit_up);
	if (val == 7) val = 6; // no left bauer possible, so right bauer is value 6/6
	temp[9 + val] = 1.0;


	// cards in hand - temp[16-39]

	for (size_t i = 0; i < _hand->size(); i++)
	{
		Card c = _hand->getCard(i);
		suit_t suit = realSuit_test(c, trump);
		int suit_offset = 0; // offset in temp vector

		if (suit == opposite1) suit_offset = 6;
		else if (suit == next) suit_offset = 12;
		else if (suit == trump) suit_offset = 17;
		// +0 +1 +2 +3 +4 +5 -- opposite2
		// +6 +7 +8 +9 +10 +11 -- opposite1
		// +12 +13 +14 +15 +16 -- next
		// +17 +18 +19 +20 +21 +22 +23 --trump
		temp[15 + suit_offset + card_to_num_test(c, trump)] = 1.0;
	}
	return temp;
}


// ****Meta data functions -----------------------------------------------------------------------

void Player_AI::newHand() // reset meta data
{
	// --- reset per hand data
	_me_dealer = false;
	_me_bidder = false;
	_partner_bidder = false;
	_discarded = false;
	_round_processed = true;

	// --- reset metadata
	_partner_has_trump = true;
	_num_cards_played = 0; // num cards played
	_cards_played.clear();  // cards played vector

	for (auto it = _suits_led.begin(); it != _suits_led.end(); it++) // suits led
	{
		*it = false;
	}

	for (auto it = _trump_remaining.begin(); it != _trump_remaining.end(); it++) // trump remaining
	{
		*it = true;
	}

	// (note: _values_remaining reset in processRound function)

	vector<int> temp(4, 6);
	_remaining_in_suit = temp; // remaining cards in suit

	// --- reset helper data
	_card_map.clear();
}

void Player_AI::processHand()
{
	for (size_t i = 0; i < _hand->size(); i++)
	{
		Card c = _hand->getCard(i);
		_remaining_in_suit[suit_to_num(c._suit)]--;
	}
}

void Player_AI::processRound()
{
	// --- set once per hand data
	_trump = _s->_trump;
	if (_trump == diamonds) _next = hearts;
	else if (_trump == clubs) _next = spades;
	else if (_trump == hearts) _next = diamonds;
	else _next = clubs;
	if (_me == _s->_dealer) _me_dealer = true;
	if (_me == _s->_bidder) _me_bidder = true;
	if (_partner == _s->_bidder) _partner_bidder = true;

	// -- reformat remaining in suit
	_remaining_in_suit[suit_to_num(_trump)]++;
	_remaining_in_suit[suit_to_num(_next)]--;
	//int upsuit = suit_to_num(_s->_up);

	// set card up
	Card cardup = _s->_cardup;

	//check if the left is gone (i.e. the right bauer was turned down and trump was decided in round 2)
	if (cardup._suit == _next && cardup._value == jack) 
	{
		_remaining_in_suit[suit_to_num(_trump)]--;
	}

	// reformat and reset values remaining
	resetValuesRemaining();

}


void Player_AI::processCard(Card c, player_t player)
{

	if (!_round_processed) processRoundData();

	suit_t suit = realSuit(c);
	_num_cards_played++; // update number of cards played


	if (player == _s->_leader)
	{
		_lead_suit = suit;
		_lead_card = c;
		_suits_led[suit_to_num(_lead_suit)] = true; //record suit lead
	}

	_cards_played.push_back(c); // record card played

	_remaining_in_suit[suit_to_num(suit)]--; // update remaining cards in each suit

	comparePlayed(); // update _top_player and _top_card

	// check if partner is out of trump
	if (player == _partner && _partner_has_trump)
	{
		if (_lead_suit == _trump && suit != _trump) // if partner did not follow trump
		{
			_partner_has_trump = false;
		}
		if (suit != _lead_suit && suit != _trump && _top_player != _me) // if partner missed opportunity to trump
		{
			_partner_has_trump = false;
		}
		if (_remaining_in_suit[suit_to_num(_trump)] == 0) // if all trump outside of hand are gone
		{
			_partner_has_trump = false;
		}
	}

	if (suit == _trump) // if trump played
	{
		int x = card_to_num(c) - 1;
		_trump_remaining[x] = false;; // remove from remaining trump
	}

	// remove from remaining values
	_values_remaining[suit_to_num(suit)][card_to_num(c) - 1] = false;

	if (_num_cards_played == 20) // last card played : round over
	{
		_round_processed = false;
	}
}

void Player_AI::recordDiscard(Card c)
{
	_discarded = true;
	_discard_card = c;
}

void Player_AI::processDiscard()
{
	Card c = _discard_card;
	suit_t suit = realSuit(c);
	if (suit == _trump) // if trump played
	{
		int x = card_to_num(c) - 1;
		_trump_remaining[x] = false;; // remove from remaining trump
	}
	_values_remaining[suit_to_num(suit)][card_to_num(c) - 1] = false;
	_remaining_in_suit[suit_to_num(suit)]--; // update remaining cards in each suit
}

void Player_AI::processRoundData()
{
	// set per hand data
	newHand();
	processRound();
	processHand();
	if (_discarded) processDiscard();
}

void Player_AI::processPlayed()									
{

	_lead_card = _played->getCard(0);
	_lead_suit = realSuit(_lead_card);
	comparePlayed();
		
}


// ****Decision functions -----------------------------------------------------------------------

bool Player_AI::suitLed(suit_t s)
{
	if (_suits_led[suit_to_num(s)]) return true;
	return false;
}

bool Player_AI::hasTrump()
{
	Deck legal = _legal_hand;
	size_t size = legal.size();
	for (size_t i = 0; i < size; i++)
	{
		if (realSuit(legal.getCard(i)) == _trump) return true;
	}
	return false;
}

bool Player_AI::hasTopTrump()
{
	/*
	cout << "Printing trump vector" << endl;
	for (auto it = _trump_remaining.begin(); it != _trump_remaining.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl;
	*/

	Card c;
	int highest_remaining = 0;
	//cout << "CHECKPOINT: SEARCHING FOR TRUMP" << endl;
	for (size_t i = 0; i < 7; i++) // find highest remaining trump
	{
		if (_trump_remaining[i]) // check if bit is set corresponding to card
		{
			highest_remaining = i + 1;
			//cout << "Highest trump remaining = " << highest_remaining << endl;
		}
	}
	for (size_t i = 0; i < _hand->size(); i++)
	{
		c = _hand->getCard(i);
		if (card_to_num(c) == highest_remaining) return true;
	}
	return false;
}

bool Player_AI::hasLeft()
{
	Card c;
	for (size_t i = 0; i < _hand->size(); i++)
	{
		c = _hand->getCard(i);
		if (c._value == jack && c._suit == _next) return true;
	}
	return false;
}

bool Player_AI::hasOffsuit()
{
	Card c;
	for (size_t i = 0; i < _hand->size(); i++)
	{
		c = _hand->getCard(i);
		if (realSuit(c) != _trump) return true;
	}
	return false;
}

// Determine if the opponent is currently winning the trick
// ---Assumes it is AI's turn
// ---Assumes comparePlayed() has been called this round
bool Player_AI::opponentWinning() 
{
	int num_played = _played->size();
	if (num_played == 0) return false; // leading

	if (num_played == 2)
	{
		if (_winner == 1) return false; // partner is winning
	}
	else if (num_played == 3)
	{
		if (_winner == 2) return false; // partner is winning
	}
	return true;
}

bool Player_AI::canBeat()
{
	Card temp;
	for (size_t i = 0; i < _legal_hand.size(); i++)
	{
		temp = _legal_hand.getCard(i);
		if (playCompare(temp, _top_card) == 1)
		{
			return true;
		}
	}
	return false;
}

bool Player_AI::hasTwoHigher() 
{
	int partner_val = card_to_num(_top_card);
	int top_val = 0;
	vector<int> in_hand(7, 0);
	if (realSuit(_top_card) != _lead_suit) // function should only be called if partner is the leader
	{
		cout << endl << "ERROR in hasTwoHigher: should only be called if partner is the leader" << endl << endl;
		return false;
	}
	Card temp;
	for (size_t i = 0; i < _legal_hand.size(); i++)
	{
		temp = _legal_hand.getCard(i);
		if (realSuit(temp) == _lead_suit)
		{
			int temp_val = card_to_num(temp);
			if (temp_val > partner_val)
			{
				in_hand[temp_val - 1] = 1;
				if (temp_val > top_val) top_val = temp_val;
			}
		}
	}
	if (top_val - partner_val > 1) // if have card 2 higher than partner
	{
		int suit_val = suit_to_num(_lead_suit);
		for (size_t i = partner_val; i < top_val; i++)
		{
			if (!in_hand[i - 1] && _values_remaining[suit_val][i - 1])
			{
				return true;
			}
		}
	}
	return false;
}

// determine if there is an unseen card higher than partner's card
bool Player_AI::unseenHigher() // UNTESTED
{
	Card c = _top_card;
	int value = card_to_num(c);
	suit_t suit = realSuit(c);
	int suit_num = suit_to_num(suit);
	int size = _values_remaining[suit_num].size();
	if (value == size) return false;
	for (size_t i = value; i < size; i++) // iterate through all values larger than the winning card
	{
		if (_values_remaining[suit_num][i])
		{
			return true;
		}
	}
	return false;
}


// Given a set of legal cards, return the index of the highest trump
int Player_AI::getHighestTrump()
{
	Deck legal = _legal_hand;
	// parameters
	int top_val = 0; // value of highest card so far
	int temp_val = 0; // value of current card
	Card temp;
	int index;
	suit_t suit;
	size_t size = legal.size();

	// loop
	for (size_t i = 0; i < size; i++)
	{
		temp = legal.getCard(i);
		suit = realSuit(temp);
		if (realSuit(temp) == _trump) // check if card is trump
		{
			temp_val = card_to_num(temp); // get current value
			if (temp_val > top_val) // compare current value to highest
			{
				top_val = temp_val;
				index = i;
			}
		}
	}
	if (top_val == 0) return -1; // no trump in hand
	return index;
}

int Player_AI::getLowestTrump()
{
	Card temp;
	int index;
	int temp_val;
	int low_val = 8;
	for (size_t i = 0; i < _legal_hand.size(); i++)
	{
		temp = _legal_hand.getCard(i);
		if (realSuit(temp) == _trump)
		{
			temp_val = card_to_num(temp);
			if (temp_val < low_val)
			{
				low_val = temp_val;
				index = i;
			}
		}
	}
	if (low_val == 8) return -1;
	return index;
}

int Player_AI::getHighestOffsuit()  
{
	Deck legal = _legal_hand;
	// parameters
	Card top_card; // highest card so far
	Card temp; // current card
	int top_val = 0; // value of highest card so far
	int temp_val = 0; // value of current card
	int index;
	suit_t suit;
	size_t size = legal.size();

	// loop
	for (size_t i = 0; i < size; i++)
	{
		temp = legal.getCard(i);
		suit = realSuit(temp);
		if (realSuit(temp) != _trump) // check if card is off suit
		{
			temp_val = card_to_num(temp); // get current value
			if (temp_val > top_val) // compare current value to highest
			{
				top_val = temp_val;
				top_card = temp;
				index = i;
			}
			else if (temp_val == top_val && top_val != 0) // if values are equal, pick card from unled suit
			{
				if (!suitLed(realSuit(temp)))
				{
					top_val = temp_val;
					top_card = temp;
					index = i;
				}
			}
		}
	}
	if (top_val == 0) return -1; // ONLY trump in hand
	return index;
}

// getlowestWinner
// returns index of lowest card that will win the trick
// assumes _legal_hand and _top_card are set correctly
int Player_AI::getLowestWinner() 
{
	Card temp;
	int index = -1;
	int val_to_beat = card_to_num(_top_card);
	int best_val = 99;
	int temp_val = 0;
	for (size_t i = 0; i < _legal_hand.size(); i++)
	{
		temp = _legal_hand.getCard(i);
		if (playCompare(temp, _top_card) == 1)
		{
			temp_val = card_to_num(temp);
			if (temp_val < best_val)
			{
					best_val = temp_val;
					index = i;
			}
		}
	}
	return index;
}

int Player_AI::getHighestFollow()
{
	Card temp;
	int index = -1;
	int top_val = 0;
	int temp_val = 0;
	for (size_t i = 0; i < _legal_hand.size(); i++)
	{
		temp = _legal_hand.getCard(i);
		if (realSuit(temp) == _lead_suit)
		{
			temp_val = card_to_num(temp);
			if (temp_val > top_val)
			{
				top_val = temp_val;
				index = i;
			}
		}
	}
	return index;
}

int Player_AI::getLowestFollow()
{
	Card temp;
	int index = -1;
	int lowest_val = 99;
	int temp_val = 0;
	for (size_t i = 0; i < _legal_hand.size(); i++)
	{
		temp = _legal_hand.getCard(i);
		if (realSuit(temp) == _lead_suit)
		{
			temp_val = card_to_num(temp);
			if (temp_val < lowest_val)
			{
				lowest_val = temp_val;
				index = i;
			}
		}
	}
	return index;
}

// get index to left bauer in legal cards
int Player_AI::getLeft()
{
	Card c;
	for (size_t i = 0; i < _legal_hand.size(); i++)
	{
		c = _legal_hand.getCard(i);
		if (c._value == jack && c._suit == _next) return i;
	}
	return -1;
}

int Player_AI::getSlough()
{
	Card c;
	suit_t temp_suit;
	//cout << "TESTING - in getSlough - Legal Hand: "; _legal_hand.print_deck(1);
	size_t size = _legal_hand.size();
	int index = 0;
	int best_quality = -1;
	int temp_quality = 0;
	int best_value = 0;
	int secondary_quality = 0;
	vector<int> suits(4, 0);
	for (size_t i = 0; i < size; i++) // find solitary cards
	{
		c = _legal_hand.getCard(i);
		suits[suit_to_num(realSuit(c))]++;
	}
	for (size_t i = 0; i < size; i++)
	{
		c = _legal_hand.getCard(i);
		temp_suit = realSuit(c);
		temp_quality = 0;
		if (realSuit(c) != _trump) // prefer offsuit
		{
			temp_quality = 1;
			if (c._value != ace) // prefer non-ace
			{
				temp_quality = 2;
				if (hasTrump()) // if has trump
				{
					if (suits[suit_to_num(temp_suit)] == 1) // prefer solitary
					{
						temp_quality = 3;
						if (!suitLed(temp_suit)) // prefer unled suit
						{
							temp_quality = 4;
						}
					}
				}
			}
		}
		if (temp_quality > best_quality)
		{
			best_quality = temp_quality;
			best_value = card_to_num(c);
			index = i;
		}
		else if (temp_quality == best_quality) // in case of equal quality, choose the lower card
		{
			int temp_value = card_to_num(c);
			if (temp_value < best_value)
			{
				best_quality = temp_quality;
				best_value = temp_value;
				index = i;
			}
		}
	}
	return index;
}


void Player_AI::findLegal()
{
	_can_follow = false;
	if (_me == _s->_leader)
	{
		_legal_hand = *_hand;
	}
	else
	{
		suit_t lead = _s->_lead;
		Deck playable;
		for (size_t i = 0; i < _hand->size(); i++)
		{
			Card c = _hand->getCard(i);
			if (realSuit(c) == lead)
			{
				_can_follow = true;
				playable.addCard(_hand->getCard(i));
			}
		}
		if (_can_follow) _legal_hand = playable;
		else _legal_hand = *_hand; // lead suit not in hand
	}
}


int Player_AI::playCard(int index)
{
		return _card_map[_legal_hand.getCard(index)];
}

double Player_AI::evaluate(vector<double> * input)
{

	return _network->evaluate(input);
}

int Player_AI::playDecide()
{
	
	// data
	size_t hand_size = _hand->size();
	if (hand_size == 1) return 1; // if only one card in hand: play it
	if (!_round_processed) processRoundData(); // first card played: process round data

	player_t leader = _s->_leader;
	
	bool partner_lead = false;
	if (leader == _partner) partner_lead = true;

	bool me_leader = false;
	if (_me == leader) me_leader = true;

	bool have_trump = false;

	// make card map for finding the card index
	_card_map.clear();
	for (size_t i = 0; i < _hand->size(); i++)
	{
		Card c = _hand->getCard(i);
		size_t index = i + 1;
		_card_map.insert(std::pair<Card, size_t>(c, index));
	}
	

	// Find legal plays:
	findLegal(); // find all legal cards to play


	size_t num_legal = _legal_hand.size();
	if (num_legal == 1) // if only one legal card: play it
	{
		
		int x = playCard(0);
		if (_display_choice)
		{
			string str = "ONLY ONE LEGAL CARD: ";
			displayChoice(str, x);
		}
		return x;

	}

	// LOGIC:
	bool has_trump = hasTrump();
	bool has_top_trump = hasTopTrump();
	bool has_left = hasLeft();
	bool has_offsuit = hasOffsuit();

	if (me_leader) // if leader
	{
		if (_me_bidder) // if bidder
		{	
			if (has_trump) // if have trump
			{
				if (!suitLed(_trump) || !_partner_has_trump) // If have not led trump yet OR partner has no trump
				{
					if (has_top_trump) // if have the top trump -> play highest trump
					{
						int index = getHighestTrump();
						if (index != -1)
						{
							int x = playCard(index);
							if (_display_choice)
							{
								string str = "CALLED TRUMP: LEADING HIGHEST TRUMP: ";
								displayChoice(str, x);
							}
							return x;
						}
					}
					// if do not have top trump -> play lowest trump (to bait out high trump)
					int index = getLowestTrump();
					if (index != -1)
					{
						int x = playCard(index);
						if (_display_choice)
						{
							string str = "CALLED TRUMP: LEADING LOWEST TRUMP: ";
							displayChoice(str, x);
						}
						return x;
					}
				}
				// --- does not have trump:
			}
			// If trump has been lead, if partner still has trump remaining, or if no trump -> play highest offsuit
			int index = getHighestOffsuit();
			if (index != -1)
			{
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "CALLED TRUMP: LEADING HIGHEST OFFSUIT: ";
						displayChoice(str, x);
					}

					return x;
				}
			}

			// If ONLY trump in hand: 
			if (has_top_trump) //If have highest trump->play highest trump
			{
				int index = getHighestTrump();
				if (index != -1)
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "CALLED TRUMP: ONLY TRUMP IN HAND: LEADING TOP TRUMP: ";
						displayChoice(str, x);
					}
					return x;
				}
			}
			index = getLowestTrump(); // Only trump, but do not have highest -> play lowest trump
			if (index != -1)
			{
				int x = playCard(index);
				if (_display_choice)
				{
					string str = "CALLED TRUMP: ONLY TRUMP IN HAND: LEADING LOWEST TRUMP: ";
					displayChoice(str, x);
				}
				return x;
			}
		}

		if (_s->_bidder == _partner) // If partner called
		{
			if (has_trump) // if have trump
			{
				if (!suitLed(_trump) || !_partner_has_trump) // If have not led trump yet OR partner has no trump
				{
					//cout << endl << "TESTING -  Trump led already? " << suitLed(_trump) << endl;
					//cout << "TESTING - Partner has trump? " << _partner_has_trump << endl;
					if (has_top_trump)
					{
						int index = getHighestTrump();
						if (index != -1)
						{
							int x = playCard(index);
							if (_display_choice)
							{
								string str = "PARTNER CALLED: LEADING HIGHEST TRUMP: ";
								displayChoice(str, x);
							}
							return x;
						}
					}
					if (has_left) // if have left bauer
					{
						int index = getLeft();
						if (index != -1)
						{
							int x = playCard(index);
							if (_display_choice)
							{
								string str = "PARTNER CALLED: LEADING LEFT BAUER: ";
								displayChoice(str, x);
							}
							return x;
						}
					}
					// Otherwise, lead lowest trump
					int index = getLowestTrump(); 
					if (index != -1)
					{
						int x = playCard(index);
						if (_display_choice)
						{
							string str = "PARTNER CALLED: LEADING LOWEST TRUMP: ";
							displayChoice(str, x);
						}
						return x;
					}
					
				}
			} // ---either trump has been led, or have no trump:
			if (has_offsuit)
			{
				int index = getHighestOffsuit();
				if (index != -1)
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "PARTNER CALLED: LEADING HIGHEST OFFSUIT: ";
						displayChoice(str, x);
					}
					return x;
				}
			} // ONLY trump in hand, but trump has been led
			if (has_top_trump)
			{
				int index = getHighestTrump();
				if (index != -1)
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "PARTNER CALLED: ONLY TRUMP LEFT -> LEADING TOP TRUMP: ";
						displayChoice(str, x);
					}
					return x;
				}
				index = getLowestTrump();
				if (index != -1)
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "PARTNER CALLED: ONLY TRUMP LEFT -> LEADING LOWEST TRUMP: ";
						displayChoice(str, x);
					}
					return x;
				}
			}
		} //  --- end: partner called (lead)
		// Other team called (lead)
		if (has_offsuit)
		{
			int index = getHighestOffsuit();
			if (index != -1)
			{
				int x = playCard(index);
				if (_display_choice)
				{
					string str = "OPPONENT CALLED: LEADING HIGHEST OFFSUIT: ";
					displayChoice(str, x);
				}
				return x;
			}
		} 
		// ONLY trump in hand
		int index = getLowestTrump();
		if (index != -1)
		{
			int x = playCard(index);
			if (_display_choice)
			{
				string str = "OPPONENT CALLED: LEADING LOWEST TRUMP: ";
				displayChoice(str, x);
			}
			return x;
		}

	} // --- END LEADING

	processPlayed();
	bool opponent_winning = opponentWinning();
	bool can_beat = canBeat();

	if (_me == _s->_leader)																	// Error check
	{
		cout << "ERROR: should have selected a card to lead" << endl;
	}

	// IF FOLLOWING
	
	if (_can_follow) // can follow
	{
		if (opponent_winning) // opponent winning
		{
			if (can_beat) // can beat opponent
			{
				if (leader == _left_opp) // if playing last card
				{
					int index = getLowestWinner();
					if (index != -1)
					{
						int x = playCard(index);
						if (_display_choice)
						{
							string str = "FOLLOWING - OPPONENT WINNING - LAST CARD: PLAYING LOWEST WINNER: ";
							displayChoice(str, x);
						}
						return x;
					}
				}
				int index = getHighestFollow();
				if (index != -1)
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "FOLLOWING - OPPONENT WINNING: PLAYING HIGHEST LEGAL CARD: ";
						displayChoice(str, x);
					}
					return x;
				}
			} // can't beat opponent
			int index = getLowestFollow();
			if (index != -1)
			{
				int x = playCard(index);
				if (_display_choice)
				{
					string str = "FOLLOWING - OPPONENT WINNING: PLAYING LOWEST LEGAL CARD: ";
					displayChoice(str, x);
				}
				return x;
			}
		}
		// can follow - partner winning
		if (leader == _partner) // if partner is leader
		{
			if (hasTwoHigher())
			{
				int index = getHighestFollow();
				if (index != -1)
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "FOLLOWING - PARTNER WINNING: PLAYING HIGHEST LEGAL CARD: ";
						displayChoice(str, x);
					}
					return x;
				}
			}
		} 
		// if partner is not leader (i.e. playing last card of trick), or do not have a card 2 higher:
		int index = getLowestFollow();
		if (index != -1)
		{
			int x = playCard(index);
			if (_display_choice)
			{
				string str = "FOLLOWING - PARTNER WINNING: PLAYING LOWEST LEGAL CARD: ";
				displayChoice(str, x);
			}
			return x;
		}
	} // -- end: can follow
	  // if can't follow
	if (opponent_winning) 
	{
		if (can_beat) // if can trump or out trump
		{
			int index = getLowestWinner(); // trump (or out trump) winning card
			if (index != -1)
			{
				int x = playCard(index);
				if (_display_choice)
				{
					string str = "FOLLOWING - OPPONENT WINNING: PLAYING LOWEST WINNING TRUMP: ";
					displayChoice(str, x);
				}
				return x;
			}
		}
		// can't beat: slough off
		// cout << endl << "TESTING - OPPONENT WINNING - SLOUGHING OFF" << endl;
		int index = getSlough();
		// cout << endl << "TESTING - about to playCard  - index: " << index << endl;
		int x = playCard(index);
		if (_display_choice)
		{
			string str = "FOLLOWING - OPPONENT WINNING: SLOUGHING OFF: ";
			displayChoice(str, x);
		}
		return x;
	}
	// can't follow - partner winning
	if (leader == _partner) // if partner leading (otherwise beating partner is meaningless)
	{
		if (has_trump) // if have trump
		{
			if (unseenHigher()) // if unseen card exists higher than partner's card 
			{						// (NOTE: Not sure if optimal. Could be replaced with "if less than king" or similar)
				int index = getLowestTrump();
				if (index != -1)
				{
					int x = playCard(index);
					if (_display_choice)
					{
						string str = "FOLLOWING - PARTNER WINNING: PLAYING LOWEST TRUMP: ";
						displayChoice(str, x);
					}
					return x;
				}
			}
		}
	}
		int index = getSlough();
		int x = playCard(index);
		if (_display_choice)
		{
			string str = "FOLLOWING - PARTNER WINNING: SLOUGHING OFF: ";
			displayChoice(str, x);
		}
		return x;
	


	cout << endl << " ---- ERROR: should have selected a card to play ---- " << endl << endl;		// Error check


	int rand_index = rand() % num_legal; // base case: random
	x = playCard(rand_index);
	cout << endl << "SELECTING RANDOMELY " << x << endl;
	//cout << "Legal cards:" << endl;
	_legal_hand.print_deck(1);
	//cout << "Number of legal cards: " << num_legal << endl;
	//cout << "Size of map: " << _card_map.size() << endl;
	return x;
	//return rand() % hand_size + 1; // base case: random
}

int Player_AI::discardDecide()
{
	processRoundData();
	Card c;
	int temp_quality = 0;
	int best_quality = -1;
	int best_value = 99;
	size_t size = _hand->size();
	int index = 0;

	vector<int> suits(4, 0);
	for (size_t i = 0; i < size; i++) // find solitary cards
	{
		c = _hand->getCard(i);
		suits[suit_to_num(realSuit(c))]++;
	}
	// look through hand
	for (size_t i = 0; i < size; i++)
	{
		c = _hand->getCard(i);
		suit_t temp_suit = realSuit(c);
		temp_quality = 0;
		if (realSuit(c) != _trump)
		{
			temp_quality = 1;
			if (c._value != ace)
			{
				temp_quality = 2;
				if (suits[suit_to_num(temp_suit)] == 1) // prefer solitary
				{
					temp_quality = 3;
				}
			}
		}
		if (temp_quality > best_quality)
		{
			best_quality = temp_quality;
			best_value = card_to_num(c);
			index = i;
		}
		else if (temp_quality == best_quality) // in case of equal quality, choose the lower card
		{
			int temp_value = card_to_num(c);
			if (temp_value < best_value)
			{
				best_quality = temp_quality;
				best_value = temp_value;
				index = i;
			}
		}
	}
	int x = index + 1;
	if (_display_choice)
	{
		cout << endl << "DISCARD DECISION - TOP QUALITY: " << best_quality;
		string str = "DISCARDING CARD: ";
		displayChoice(str, x);
	}
	return x;
}

int Player_AI::bidDecide()
{
	double quality = 0;
	vector<double> input;
	bool round2 = _s->_round2;
	if (!round2)
	{
		input = makeInput();
		quality = evaluate(&input);
		//cout << "Quality: " << quality << endl;
		if (quality > 0.5) return 1;
	}
	else
	{
		double temp_quality = 0;
		int num_suit_chosen = 0;
		int suit_counter = 0;
		suit_t turned_down = _s->_up;
		suit_t curr_suit = diamonds;
		while (true)
		{
			if (turned_down != curr_suit)
			{
				suit_counter++;
				input.clear();
				input = makeInput(true, curr_suit);
				/*
				cout << endl << "Input vector: " << endl;
				for (auto it = input.begin(); it != input.end(); it++)
				{
					cout << *it << " ";
				}
				*/
				temp_quality = evaluate(&input);
				//cout << "Quality: " << temp_quality << endl;
				if (temp_quality > quality)
				{
					quality = temp_quality;
					num_suit_chosen = suit_counter;
				}
			}
			if (curr_suit == diamonds) curr_suit = clubs;
			else if (curr_suit == clubs) curr_suit = hearts;
			else if (curr_suit == hearts) curr_suit = spades;
			else break;
		}
		if (quality > 0.5) return num_suit_chosen;
	}

	
	return 0;
}
