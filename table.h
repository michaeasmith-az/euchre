#ifndef _table_h_included_
#define _table_h_included_

#include "state.h"


class Table{
public:
	Table();
	Table(State * state);
	//void shuffle();
	//void draw(Deck hand);
	void deal();
	void playCard(Deck * hand, size_t index);
	void playCard(player_t player, size_t index);
	void discard(Deck * hand, size_t index);
	void discard(player_t player, size_t index);
	void removePlayed();
	void pickUp(Deck hand);
	void pickUp(player_t player);
	int handCompare(Card a, Card b);
	int playCompare(Card a, Card b);
	int comparePlayed();
	void sort(Deck hand);
	void sort(player_t player);
	void sort();
	void showTop();
	suit_t topSuit();
	Card topCard();
	void showHand(player_t player);
	void showPlayed();
	int handSize(player_t player);
	Deck * hand(player_t player);
	suit_t getSuit(player_t player, int index);
	bool checkSuit(player_t player, suit_t suit);
	Card getCard(player_t player, int index); // UNTESTED
	Card getPlayed();
	void clear();
	
	
	State * _state; // not owned by *this
	Deck _deck;
	Deck _N_hand;
	Deck _E_hand;
	Deck _S_hand;
	Deck _W_hand;
	Deck _inPlay;
};

#endif

