#ifndef _player_h_included_
#define _player_h_included_

#include "table.h"
#include "player_ai.h"
#include <map>
using std::map;

class Player {
public:
	Player(Table * t = nullptr, player_t side = S, const string name = "Player 1", const string name2 = "Player 2", 
		const string name3 = "Player 3", const string name4 = "Player 4");
	Player(FFNN_Evo * network, Table * t = nullptr, player_t side = S, const string name = "Player 1", const string name2 = "Player 2",
		const string name3 = "Player 3", const string name4 = "Player 4");
	Player(const Player &old);
	Player& Player::operator=(const Player & old);
	~Player();
	void showP(player_t p);
	void showS(suit_t s);
	player_t nextPlayer(player_t p);
	int processInput(int first_choice, int last_choice, string input);
	bool followSuit(int index);
	void header();
	void visualHeader();
	void specificHeader();
	void bidHeader();
	void playHeader();
	int bid1();
	int discard();
	int bid2();
	int playCard();
	void recordPlayed(player_t current);
	int decision();
	

	bool followSuit_AI(int index);
	int bid1_AI();
	int discard_AI();
	int bid2_AI();
	int playCard_AI();
	
	

	Table * _t; // not owned by *this
	State s;
	bool _real;
	string _name;
	string _name2;
	string _name3;
	string _name4;
	player_t _me;
	map<player_t, string> _players;


	Player_AI  *_ai; // owned by *this
};

#endif
