#include "deck.h"

Card::Card() : _value(ace), _suit(diamonds) {
}

Card::Card(value_t v, suit_t s): _value(v), _suit(s){
}

Deck::Deck(int num_suits, value_t first_val){
	if (num_suits == 0)
	{
		return;
	}
	else
	{
		//Card * t;
		for (int i = 1; i < num_suits + 1; ++i)
		{
			for (int j = first_val; j < 14; ++j)
			{
				_cards.push_back(new Card(value_t(j), suit_t(i)));
			}
			
		}
	}
}

Deck::Deck(const Deck & old) : _cards(old._cards.size())
{
	for (size_t i = 0; i < old._cards.size(); i++)
	{
		_cards[i] =  new Card(*old._cards[i]);
	}
}

Deck& Deck::operator=(const Deck & old)
{
	_cards.resize(old._cards.size());
	for (size_t i = 0; i < old._cards.size(); i++)
	{
		_cards[i] = new Card(*old._cards[i]);
	}
	return *this;
}

Deck::~Deck()
{
	for (auto it = _cards.begin(); it != _cards.end(); it++)
	{
		delete *it;
	}
	_cards.clear();
}

size_t Deck::size() {
	return _cards.size();
}

void Deck::shuffle()
{
	//std::shuffle(_cards.begin(), _cards.end(), engine);
	int n = std::distance(_cards.begin(), _cards.end());
	for (int i = n - 1; i > 0; --i) {
		using std::swap;
		std::swap(_cards[i], _cards[std::rand() % (i + 1)]);
	}
}


void Deck::print_deck(int options) {
	if (options > 1 || options < 0 ) options = 1;
	if (_cards.size() > 0)
	{
		for (size_t i = 0; i < _cards.size() - 1; ++i)
		{
			print_card(i);
			if (options == 1) cout << " , ";
			else cout << endl;
		}
		print_card(_cards.size() - 1);
	}
	cout << endl;
}

// Pre: i is within deck size
// Post: prints card suit and value to console
void Deck::print_card(const size_t i) {
	print_card(*_cards[i]);
}


void Deck::print_card(const Card c){
	switch(c._value){
	case 1:
		cout << 'A';
		break;
	case 2: 
		cout << '2';
		break;
	case 3: 
		cout << '3';
		break;
	case 4:
		cout << '4';
		break;
	case 5:
		cout << '5';
		break;
	case 6:
		cout << '6';
		break;
	case 7:
		cout << '7';
		break;
	case 8:
		cout << '8';
		break;
	case 9:
		cout << '9';
		break;
	case 10:
		cout << "10";
		break;
	case 11:
		cout << 'J';
		break;
	case 12:
		cout << 'Q';
		break;
	case 13:
		cout << 'K';
	}
	cout << ' ';
	switch (c._suit){
	case 1:
		cout << 'D';
		break;
	case 2:
		cout << 'C';
		break;
	case 3:
		cout << 'H';
		break;
	case 4:
		cout << 'S';
	}
}

int Deck::find(value_t v, suit_t s) {
	for (int i = 0; i < _cards.size(); ++i)
	{
		Card c = *_cards[i];
		if (c._value == v && c._suit == s)
		{
			return i;
		}
	}
	return -1;
}

Card * Deck::remove_top() {
		Card * temp = _cards.back();
		_cards.pop_back();
		return temp;
}

void Deck::add_top(Card * c) {
	_cards.push_back(c);
}

void Deck::add_bottom(Card * c) {

	_cards.insert(_cards.begin(), c);
}

void Deck::addCard(value_t v, suit_t s)
{
	_cards.push_back(new Card(v, s));
}

void Deck::addCard(Card c)
{
	addCard(c._value, c._suit);
}

Card * Deck::removeCard(size_t index) 
{
	Card * t = _cards[index];

	_cards.erase(_cards.begin() + index);

	return t;
}

suit_t Deck::getSuit(size_t index)
{
	return _cards[index]->_suit;
}

Card Deck::getCard(size_t index)
{
	return *_cards[index];
}