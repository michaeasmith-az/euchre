#ifndef _state_h_included_
#define _state_h_included_

#include "deck.h"

enum phase_t {bid, play};
enum player_t {W = 1, N = 2, E = 3, S = 4};

struct State {
public:
	int _score1 = 0; // N, S
	int _score2 = 0; // E, W
	int _tricks1 = 0;
	int _tricks2 = 0;
	bool _trump_flag = false;
	suit_t _trump = diamonds;
	suit_t _lead = diamonds;
	suit_t _up = diamonds;
	Card _cardup;
	phase_t _phase = bid;
	bool _round2 = false;
	player_t _dealer = N;
	player_t _leader = N; // player who has the lead
	player_t _bidder = N; // player who bid
	// top card played?
	// index of top card played?
	// winning player?

	// Data of each player's bid, in round 1 and round 2?
};

#endif
