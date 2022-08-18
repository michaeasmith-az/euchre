#ifndef _deck_h_included_
#define _deck_h_included_

#include<iostream>
using std::cout;
using std::cin;
using std::endl;
#include<string>
using std::string;
#include<vector>
using std::vector;
#include <stdio.h>      //NULL
#include <stdlib.h>     // srand, rand
#include <time.h>       //

using std::size_t;

//auto engine = std::default_random_engine{};
enum value_t {ace = 1, two = 2, three = 3, four = 4, five = 5, six = 6, seven = 7, eight = 8, nine = 9, ten = 10, jack = 11, queen = 12, king = 13};
enum suit_t{diamonds = 1, clubs = 2, hearts = 3, spades = 4};

class Card{
public:
	Card();
	Card(value_t v, suit_t s = diamonds);
	value_t _value;
	suit_t _suit;
};



class Deck{
public:
	
	Deck(int num_suits = 0, value_t first_val = ace);
	Deck(const Deck & old);
	Deck& Deck::operator=(const Deck & old);
	~Deck();
	size_t size();
	void shuffle();
	void print_card( const size_t i);
	void print_card(const Card c);
	void print_deck(int options = 1);
	int find(value_t v, suit_t s);
	Card * remove_top();
	void add_top(Card * c);
	void add_bottom(Card * c);
	void addCard(value_t v, suit_t s);
	void addCard(Card c);
	Card * removeCard(size_t index);
	suit_t getSuit(size_t index);
	Card getCard(size_t index);


	vector<Card*> _cards; // owned by *this
};



#endif