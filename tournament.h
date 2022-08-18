#ifndef _tournament_h_included_
#define _tournament_h_included_

#include "game.h"
#include <algorithm> // for random_shuffle

using std::fstream;

class Tournament
{
public:
	Tournament(int num_nets = 0);
	void initializeGen1();
	void readNets(int gen);
	void playGames(int num_games_per_net);
	void nextGen(int gen);
	void run();

	int _num_weights = 8450;
	int _num_nets;
	vector<int> _network;
	vector<FFNN_Evo*> _netvec; // pointers to networks in _nets
	vector<FFNN_Evo> _nets;
	vector<int> _scores;
};


#endif
