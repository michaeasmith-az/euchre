#ifndef _player_ai_h_included_
#define _player_ai_h_included_

#include "table.h"
#include "ffnn.h"
#include "evolver.h"
#include "ffnn_evo.h"
#include <map>
//#include "player.h"
using std::map;

struct CardCompare
{
	bool operator() (const Card & a, const Card & b) const
	{
		if (a._suit < b._suit) return true;
		if (b._suit < a._suit) return false;

		return (a._value < b._value);
	}
};

class Player_AI {

public:
	Player_AI(State *s, player_t me, Deck * hand, Deck * played, FFNN_Evo * network);

	//testing output:
	bool _display_choice = false;

	// helper functions:
	player_t nextPlayer(player_t p);
	int suit_to_num(suit_t s);
	int card_to_num(Card c);
	int card_to_num_test(Card c, suit_t trump);
	suit_t realSuit(Card c);
	suit_t realSuit_test(Card c, suit_t trump);
	void displayChoice(string message, int index);
	int playCompare(Card a, Card b); // UNTESTED
	void comparePlayed(); // UNTESTED
	void resetValuesRemaining(); // UNTESTED
	int getTeamScore();
	int getOppScore();
	vector<double> makeInput(bool round2 = false, suit_t trump_in_question = diamonds);
	

	// meta data functions:
	void newHand();
	void recordDiscard(Card c); // UNTESTED
	void processHand(); // IN TESTING
	void processRound(); // IN TESTING
	void processCard(Card c, player_t player); // IN TESTING
	void processDiscard();
	void processRoundData();
	void processPlayed();
	
	// decision functions:

	// logic functions:
	bool suitLed(suit_t s);
	bool hasTrump();
	bool hasTopTrump();
	bool hasLeft();
	bool hasOffsuit();
	bool opponentWinning(); // UNTESTED
	bool canBeat(); // UNTESTED
	bool hasTwoHigher(); // UNTESTED
	bool unseenHigher(); // UNTESTED

	// index functions:
	int getHighestTrump();
	int getLowestTrump();
	int getHighestOffsuit();
	int getLowestWinner();
	int getHighestFollow(); // UNTESTED
	int getLowestFollow(); // UNTESTED
	int getLeft();
	int getSlough();

	void findLegal();
	int playCard(int index);
	double evaluate(vector<double> * input);

	int playDecide();
	int discardDecide();
	int bidDecide();
	


	// DATA:

	// set once per game
	State *_s;
	Deck *_hand;
	Deck * _played;
	player_t _me;
	player_t _left_opp;
	player_t _partner;
	player_t _right_opp;
	

	// set once per hand
	suit_t _trump;
	suit_t _next;
	bool _me_dealer;
	bool _me_bidder;
	bool _partner_bidder;
	bool _discarded;
	Card _discard_card;
	bool _round_processed;

	// set once per round
	suit_t _lead_suit; // set in processCard
	Card _lead_card; // set in processCard


	// Meta data:
	bool _partner_has_trump;
	int _num_cards_played;
	vector<Card> _cards_played;
	vector<bool> _suits_led; // reset in newHand, set in processCard
	vector<bool> _trump_remaining;
	vector<vector<bool>> _values_remaining;
	vector<int> _remaining_in_suit; // vector of length 4 storing the number of unseen cards in each suit
	bool _can_follow; // set in findLegal
	int _winner; // position of player currently winning the trick
			// used for deciding card to play when not the leader
			// set in comparePlayed
	Card _top_card; // set in comparePlayed
	player_t _top_player; // set in comparePlayed
	

	// helper data
	// used for decisions
	map<Card, size_t, CardCompare> _card_map; // reset in newHand, set in playDecide
	Deck _legal_hand; // set in findLegal

	// neural network
	FFNN_Evo * _network; // not owned by *this



};

#endif
