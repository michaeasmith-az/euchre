#ifndef _game_h_included_
#define _game_h_included

#include "table.h"
#include "player.h"
#include "player_ai.h"

class Game {
public:
	Game();
	void setNetworks(vector<FFNN_Evo *> & netvec);
	void deal();
	void sort();
	void newHand();
	player_t nextPlayer(player_t p);
	void playCard(player_t player, size_t index);
	void discard(player_t player, size_t index);
	void pickUp();
	Player * player(player_t p);
	bool bid1();
	bool bid2();
	void playHand();
	void newGame(int numReal = 1);
	

	void cleanUp(); // testing only

	State _s;
	Table _t;
	Player _p1; // west
	Player _p2; // north
	Player _p3; // east
	Player _p4; // south
	int _numPlayers;
	int _numReal;
	vector<Deck> cards_played;
	vector<FFNN_Evo *> _netvec; // not owned by *this
	int _prev_score_NS = 0;
	int _prev_score_WE = 0;


};

#endif
