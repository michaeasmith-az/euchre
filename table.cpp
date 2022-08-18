#include "table.h"

Table::Table():_state(nullptr)
{

}

Table::Table(State * state): _state(state) // Make the deck
{
	Deck t(4, nine);
	t.addCard(ace, diamonds);
	t.addCard(ace, clubs);
	t.addCard(ace, hearts);
	t.addCard(ace, spades);
	_deck = t;
}

void Table::deal() // Shuffle and deal Euchre
{
	_deck.shuffle();
	for (int i = 0; i < 5; i++) {
		_W_hand.add_top(_deck.remove_top());
		_N_hand.add_top(_deck.remove_top());
		_E_hand.add_top(_deck.remove_top());
		_S_hand.add_top(_deck.remove_top());
	}
}

void Table::playCard(Deck * hand, size_t index) // Play a card
{
	_inPlay.add_top(hand->removeCard(index));
}

void Table::playCard(player_t player, size_t index)
{
	playCard(hand(player), index);
}

void Table::discard(Deck * hand, size_t index)
{
	_deck.add_bottom(hand->removeCard(index));
}

void Table::discard(player_t player, size_t index)
{
	discard(hand(player), index);
}

void Table::removePlayed()
{
	size_t size = _inPlay.size();
	for (size_t i = 0; i < size; i++)
	{
		discard(&_inPlay, 0);
	}
}

void Table::pickUp(Deck hand)
{
	hand.add_top(_deck.remove_top());
}


void Table::pickUp(player_t player)
{
	hand(player)->add_top(_deck.remove_top());
	//_N_hand.add_top(_deck.remove_top());
}

int Table::handCompare(Card a, Card b)// return 1: a farther right, 2: b farther right
{
	if (!_state->_trump_flag) // before trump declared
	{
		// compare suit
		if (a._suit > b._suit) return 1;
		if (a._suit < b._suit) return 2;
		
		// same suit, compare value
		if (a._value == ace) return 1;
		if (b._value == ace) return 2;
		if (a._value > b._value) return 1;
		return 2;
	}
	suit_t trump = _state->_trump;
	suit_t next = diamonds;
	if (trump == diamonds) next = hearts;
	if (trump == clubs) next = spades;
	if (trump == spades) next = clubs;
	

	// Special cases for bauers
	bool aRight = false;
	bool bRight = false;
	bool aLeft = false;
	bool bLeft = false;
	if (a._value == jack && a._suit == trump) aRight = true;// a is right bauer
	else if (b._value == jack && b._suit == trump) bRight = true; // b is right bauer
	if (a._value == jack && a._suit == next) aLeft = true; // a is left bauer 
	else if (b._value == jack && b._suit == next) bLeft = true; // b is left bauer 


	if (aLeft) // a is left bauer
	{
		if (b._suit != trump) return 2; // b is off trump
		if (bRight) return 2; // b is the right
		return 1; // b is lower trump
	}
	if (bLeft)
	{
		if (a._suit != trump) return 1; // a is off trump
		if (aRight) return 1; // a is the right
		return 2; // a is lower trump
	}

	if (a._suit != b._suit) // different suits
	{
		if (a._suit == trump) return 2; // a is trump
		if (b._suit == trump) return 1; // b is trump
		if (a._suit > b._suit) return 1; // both off trump, a is higher suit
		return 2; // both off trump, a is higher suit
	}

	// same suit, no left bauer:
	if (a._suit == trump) // if both trump
	{
		if (aRight) return 1;
		if (bRight) return 2;
	}

	// same suit, no bauer:
	if (a._value == ace) return 1;
	if (b._value == ace) return 2;

	// same suit, no bauer, no ace:
	if (a._value > b._value) return 1; 
	return 2;

}

int Table::playCompare(Card a, Card b) // return 0: equal, 1: a greater, 2: b greater
{
	// in play
	// bauer present
	suit_t trump = _state->_trump;
	suit_t next = diamonds;
	if (trump == diamonds) next = hearts;
	if (trump == clubs) next = spades;
	if (trump == spades) next = clubs;

	if (a._value == jack && a._suit == trump) return 1; // a is right bauer
	if (b._value == jack && b._suit == trump) return 2; // b is right bauer
	if (a._value == jack && a._suit == next) return 1; // b is left bauer 
	if (b._value == jack && b._suit == next) return 2; // b is left bauer 

	// no bauer present
	
	if (a._suit != b._suit) // different suit
	{
		if (a._suit == _state->_trump) return 1;
		if (b._suit == _state->_trump) return 2;
		if (a._suit == _state->_lead) return 1;
		if (b._suit == _state->_lead) return 2;
		return 0; // each non-trump, non-lead suit
	}
	//same suit
	if (a._value == ace) return 1;
	if (b._value == ace) return 2;
	if (a._value > b._value) return 1;
	return 2;
}


int Table::comparePlayed() // returns 1-4 indicating whether the first-fourth card wins
{
	Card a = *(_inPlay._cards[0]); // greatest card so far
	Card b;
	int winner = 1; // winner of the trick so far

	for (size_t i = 1; i < _inPlay.size(); i++)
	{
		b = *(_inPlay._cards[i]);
		if (playCompare(a, b) == 2) // if the second is greater
		{
			a = b; // new greatest
			winner = i + 1; // new winner
		}
	}

	return winner;
}


void Table::sort(Deck hand)
{
	for (int i = 1; i < hand._cards.size(); i++) //insertion sort
	{
		int j = i;
		while (j > 0 && handCompare(*hand._cards[j - 1], *hand._cards[j]) == 1)
		{
			std::swap(*hand._cards[j - 1], *hand._cards[j]);
			j--;
		}
	}

}

void Table::sort(player_t player)
{
	 sort(*(hand(player)));
}

void Table::sort() // sort all hands
{
	sort(_W_hand);
	sort(_N_hand);
	sort(_E_hand);
	sort(_S_hand);
}

void Table::showTop()
{
	size_t size = _deck.size();
	if (size > 0)
	{
		_deck.print_card(size - 1);
	}
}

suit_t Table::topSuit()
{
	suit_t s = _deck.getSuit(_deck.size() - 1);

	return s;
}

Card Table::topCard()
{
	Card c = _deck.getCard(_deck.size() - 1);

	return c;
}

void Table::showHand(player_t player)
{
	hand(player)->print_deck(1);
}

void Table::showPlayed()
{
	_inPlay.print_deck(1);
}

int Table::handSize(player_t player)
{
	return hand(player)->size();
}

Deck * Table::hand(player_t player)
{
	if (player == W) return &_W_hand;
	if (player == N) return &_N_hand;
	if (player == E) return &_E_hand;
	else return &_S_hand;
}

suit_t Table::getSuit(player_t player, int index)
{
	if (!_state->_trump_flag) // if no trump, don't worry about bauers
	{
		return hand(player)->getSuit(index);
	}

	// if trump is called, check for left bauer

	Card t = hand(player)->getCard(index);
	suit_t trump = _state->_trump;
	suit_t next = diamonds;
	if (trump == diamonds) next = hearts;
	if (trump == clubs) next = spades;
	if (trump == spades) next = clubs;

	if (t._suit == next && t._value == jack)
	{
		return trump; // left bauer
	}
	return t._suit; // not left bauer
	
}

bool Table::checkSuit(player_t player, suit_t suit)
{
	int size = handSize(player);
	for (int i = 0; i < size; i++)
	{

		if (getSuit(player, i) == suit) return true;

	}
	return false;
}

Card Table::getCard(player_t player, int index) 
{
	return hand(player)->getCard(index);
}

Card Table::getPlayed()
{
	size_t size = _inPlay.size();
	Card c = _inPlay.getCard(size - 1);
	return c;
}

void Table::clear() // used for testing
{
	int size = _W_hand.size();
	for (int i = 0; i < size; i++)
	{
		discard(&_W_hand, 0);
	}

	size = _N_hand.size();
	for (int i = 0; i < size; i++)
	{
		discard(&_N_hand, 0);
	}

	size = _E_hand.size();
	for (int i = 0; i < size; i++)
	{
		discard(&_E_hand, 0);
	}

	size = _S_hand.size();
	for (int i = 0; i < size; i++)
	{
		discard(&_S_hand, 0);
	}
	removePlayed();
}
