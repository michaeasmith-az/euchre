#include "game.h"
#include <chrono>

void testEvolver()
{
	//cout << " * * TESTING EVOLVER * * " << endl << endl;
	int num_layers = 5;
	int num_input = 40;
	int num_weights1 = 100;
	int num_weights2 = 40;
	int num_weights3 = 10;
	vector<int> network = { num_layers, num_input, num_weights1, num_weights2, num_weights3, 1 };
	int num_weights = num_input + num_input*num_weights1 + num_weights1*num_weights2 + num_weights2*num_weights3 + num_weights3;
	//cout << "Number of weights: " << num_weights << endl;

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
	0 1 = same color ("next")
	1 0 = opposite color
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
	vector<double> input_weights = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,			// game state
		1.1, 1.2, 1.3, 1.4, 1.5, 1.6,								// card up 
		1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, // offsuit 1 and 2
		1.1, 1.2, 1.3, 1.4, 1.5,									// next suit
		2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7 };						// trump suit

																	// for this example, assumes hearts = trump
	vector<double> sample_input = { 0.0, 0.0, // score 0 to 0
		1.0, 0.0, 0.0, 0.0, // left of dealer
		1.0, 0.0, // first round
		0.0, 0.0, // first round
		0.0, 0.0, 0.0, 0.0, 0.0, 1.0, // jack of trump turned up
		0.0, 0.0, 0.0, 0.0, 0.0, 1.0, // ace of spades in hand
		0.0, 0.0, 0.0, 0.0, 1.0, 0.0, // king of clubs in hand
		0.0, 0.0, 0.0, 0.0, 0.0, // no diamonds
		0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, // queen, king, ace of hearts					
	};

	FFNN net_t(network, input_weights); // generate network with random weights
										//FFNN net_t(network); // generate network with random weights
	FFNN_Evo net(net_t); // convert network to evolution network with sigma = 0.05 for every weight
	//net.writeToFile("net1gen1");
	double result = net.evaluate(&sample_input);
	//cout << "Original evaluation of sample input: " << result << endl;
	net.writeToFile("evolveTestFile1");


	Evolver e(num_weights);

	FFNN_Evo net2 = e.evolveNetwork(&net); // generate new child network
	net2.writeToFile("evolveTestFile2");
	//FFNN_Evo net3("evolveTestFile2");
	//net3.writeToFile("evolveTestFile3");
	double result2 = net2.evaluate(&sample_input);
	//cout << "Evaluation of sample input with evolved network: " << result2 << endl;
	//testNormalDistribution(net, net3);

}

void test_deck()
{

	cout << endl << "****TESTING DECK" << endl << endl;

	Deck d(4); // test Deck constructor

	d.print_deck(1); // test print_deck

	cout << endl;

	// test remove_top
	d.print_card(d.size() - 1);

	cout << endl;

	d.remove_top();

	cout << endl;

	d.print_card(d.size() - 1);

	cout << endl << endl << "Testing addCard: " << endl;

	//test add_top
	//Card c(king, spades);
	//d.add_top(&c);
	d.addCard(king, spades);

	cout << endl;

	d.print_card(d.size() - 1);

	cout << endl;

	//test removeCard

	d.print_card(40);

	cout << endl;

	d.removeCard(40);

	cout << endl;

	d.print_card(40);

	cout << endl;

	cout << endl << "****End of Deck Testing" << endl << endl;
}

void test_table()
{
	cout << endl << "****TESTING TABLE" << endl << endl;

	Game g;
	Table t = g._t;

	cout << endl << endl << "Testing compare played " << endl << endl;


	t._state->_trump_flag = true;
	t._state->_trump = hearts;

	t._inPlay.addCard(ace, spades);
	t._inPlay.addCard(king, spades);
	t._inPlay.addCard(nine, spades);
	t._inPlay.addCard(ten, hearts);
	cout << "Comparison test. Should be 4: " << t.comparePlayed(); cout << endl;



	t._deck.print_deck(1);

	cout << endl << endl << "Testing deck size: " << t._deck.size() << endl;
	
	cout << endl << endl << "Testing deal: " << endl;
	t.deal();
	
	cout << endl << "Deck: ";
	t._deck.print_deck(1);
	cout << endl << "N hand: ";
	t._N_hand.print_deck(1);
	cout << endl << "S hand: ";
	t._S_hand.print_deck(1);
	
	cout << endl << endl << "Testing sort: " << endl;
	t.sort(t._N_hand);
	t.sort(S);

	cout << endl << "Deck: ";
	t._deck.print_deck(1);
	cout << endl << "N hand: ";
	t._N_hand.print_deck(1);
	cout << endl << "S hand: ";
	t._S_hand.print_deck(1);

	t._state->_trump_flag = true;
	t._state->_trump = diamonds;

	cout << endl << endl << "Testing sort with trump = diamonds: " << endl;
	t.sort(t._N_hand);
	t.sort(S);

	cout << endl << "Deck: ";
	t._deck.print_deck(1);
	cout << endl << "N hand: ";
	t._N_hand.print_deck(1);
	cout << endl << "S hand: ";
	t._S_hand.print_deck(1);
	
	cout << endl << endl << "Testing showHand: " << endl;

	t.showHand(N);

	cout << endl << endl << "Testing showTop: " << endl;

	cout << "Deck: ";
	t._deck.print_deck(1);
	cout << "Top: "; t.showTop();

	cout << endl << endl << "Testing pickUp: " << endl;

	t.pickUp(N);
	t.showHand(N);

	cout << endl << endl << "Testing discard: " << endl;

	t.discard(N, 0);
	t.showHand(N);

	cout << endl << endl << "Testing Clear: " << endl;
	t.clear();
	cout << endl << "Deck: ";
	t._deck.print_deck(1);
	cout << endl << "N hand: ";
	t._N_hand.print_deck(1);
	cout << endl << "S hand: ";
	t._S_hand.print_deck(1);
	
	cout << endl << endl << "Testing playCard: " << endl;

	t.deal();

	cout << "North hand: "; t._N_hand.print_deck(1);
	cout << "In play: "; t._inPlay.print_deck(1);
	cout << "Playing card... " << endl;
	t.playCard(N, 0);
	cout << "North hand: "; t._N_hand.print_deck(1);
	cout << "In play: "; t._inPlay.print_deck(1);
	cout << "Playing card... " << endl;
	t.playCard(N, 0);
	cout << "North hand: "; t._N_hand.print_deck(1);
	cout << "In play: "; t._inPlay.print_deck(1);

	
	cout << endl << endl << "****End of Table Testing" << endl << endl;
}

void test_player()
{
	//cout << endl << "****TESTING PLAYER" << endl << endl;

	Game g;
	Table * t = &(g._t);
	//Table temp;
	//Table * t = &temp;
	g.deal();
	const string name = "Bob";
	Player p (t, S, name);
	//t._state->_dealer = S;
	//p.decision();


	//cout << endl << endl << "****End of Player Testing" << endl << endl;
}

void test_game()
{
	cout << endl << "****TESTING GAME" << endl << endl;
	Game g;
	
	/*
	FFNN_Evo net("net1gen1");
	vector<FFNN_Evo*> netvec = { &net, &net, &net, &net };
	g.setNetworks(netvec);

	
	for (int i = 0; i < 100; i++)
	{
		g.newGame(0);
	}
	cout << "Done" << endl;
	*/
	
	FFNN_Evo net1("nets/gen300net91");
	FFNN_Evo net2("nets/gen300net95");
	FFNN_Evo net3("nets/gen300net99");
	vector<FFNN_Evo *> netvec = { &net1, &net2, &net3 };
	g.setNetworks(netvec);
	g.newGame(1);
	


	/*
	auto t1 = std::chrono::high_resolution_clock::now(); // start timing

	g.newGame(0);

	auto t2 = std::chrono::high_resolution_clock::now(); // end timing

	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

	cout << "1 game took: "
		<< time
		<< " nanoseconds\n";

	auto eval_sec = (1e+9) / time; // eval per sec

	cout << "Games per second: " << eval_sec << endl;
	*/
}

void test_ai()
{

	cout << endl << "****TESTING PLAYER_AI" << endl << endl;
	Game g;
	Table t = g._t;
	State s = *t._state;
	Deck hand;
	Deck played;
	player_t me = S;
	player_t partner = N;

	
	s._trump = clubs;
	s._dealer = me;
	s._bidder = me;
	s._leader = E;
	s._lead = spades;


	Card c(ten, clubs);
	s._cardup = c;
	s._up = c._suit;

	// Played cards:
	 Card lead(jack, spades);
	 played.addCard(lead);
	// Card second(nine, hearts);
	// played.addCard(second);

	// Cards in hand:
	hand.addCard(jack, hearts);
	hand.addCard(nine, hearts);
	hand.addCard(jack, clubs);
	hand.addCard(ten, diamonds);
	hand.addCard(ten, hearts);
	//hand.addCard(ten, clubs);

	FFNN_Evo net1("net1gen1");
	Player_AI ai(&s, me, &hand, &played, &net1);
	ai.processRound();
	ai._lead_card = lead;
	ai._top_card = lead;
	ai.processPlayed();

	
	cout << "Original hand:" << endl;
	ai._hand->print_deck(1);
	cout << endl;
	cout << "Hand size: " << ai._hand->size() << endl;

	//int decision = ai.discardDecide();
	//cout << "Discard Decision: " << decision << endl;

	
	cout << "Played cards:" << endl;
	ai._played->print_deck(1);
	cout << endl;
	
	cout << endl << endl << "Testing findlegal: " << endl;

	cout << "Legal cards: " << endl;
	ai.findLegal();
	ai._legal_hand.print_deck(1);
	cout << endl;
	cout << "Legal size: " << ai._legal_hand.size() << endl;


	cout << endl << endl << "----------------------------------------Testing Logic Suite:: " << endl;

	cout << "Trump suit: " << ai._trump << endl;
	cout << "---->Testing hasTrump:: " << endl;
	if (ai.hasTrump())
	{
		cout << "HAS TRUMP" << endl;
	}
	else cout << "DOES NOT HAVE TRUMP" << endl;

	cout << "---->Testing hasTopTrump:: " << endl;
	if (ai.hasTopTrump())
	{
		cout << "HAS TOP TRUMP!!!" << endl;
	}
	else cout << "DOES NOT HAVE TOP TRUMP" << endl;

	cout << "---->Testing hasLeft:: " << endl;
	if (ai.hasLeft())
	{
		cout << "HAS LEFT" << endl;
	}
	else cout << "DOES NOT HAVE LEFT" << endl;

	cout << "---->Testing hasOffsuit:: " << endl;
	if (ai.hasOffsuit())
	{
		cout << "HAS OFFSUIT" << endl;
	}
	else cout << "DOES NOT HAVE OFFSUIT" << endl;

	cout << "---->Testing opponentWinning:: " << endl;
	if (ai.opponentWinning())
	{
		cout << "OPPONENT WINNING" << endl;
	}
	else cout << "OPPONENT NOT WINNING" << endl;

	if (ai._top_player != ai._partner)
	{
		cout << "---->Testing canBeat:: " << endl;
		if (ai.canBeat())
		{
			cout << "CAN BEAT" << endl;
		}
		else cout << "CANNOT BEAT" << endl;
	}
	else cout << endl << "Skipping test: canBeat - opponent is not top player" << endl << endl;

	if (ai._top_player == ai._partner)
	{
		cout << "---->Testing hasTwoHigher:: " << endl;
		if (ai.hasTwoHigher())
		{
			cout << "HAS CARD TWO HIGHER THAN PARTNER'S" << endl;
		}
		else cout << "DOES NOT HAVE CARD TWO HIGHER THAN PARTNER'S" << endl;

		cout << "---->Testing unseenHigher:: " << endl;
		if (ai.unseenHigher())
		{
			cout << "UNSEEN HIGHER CARD EXISTS" << endl;
		}
		else cout << "NO UNSEEN HIGHER CARD" << endl;
	}
	else cout << endl << "Skipping tests: hasTwoHigher and unseenHigher - partner is not top player" << endl << endl;


	cout << endl << endl << "----------------------------------------Testing Index Functions:: " << endl << endl;

	cout << "Legal cards: " << endl;
	ai.findLegal();
	ai._legal_hand.print_deck(1);


	cout << endl << "Testing getHighestTrump: " << endl;

	int index = ai.getHighestTrump();

	cout << "Highest Trump found at index: " << index << endl;


	cout << endl << "Testing getLowestTrump: " << endl;

	index = ai.getLowestTrump();

	cout << "Lowest Trump found at index: " << index << endl;


	cout << endl << "Testing getHighestOffsuit: " << endl;

	index = ai.getHighestOffsuit();

	cout << "Highest Offsuit found at index: " << index << endl;


	cout << endl << "Testing getLowestWinner: " << endl;

	index = ai.getLowestWinner();

	cout << "Lowest Winner found at index: " << index << endl;


	cout << endl << "Testing getSlough: " << endl;

	index = ai.getSlough();

	cout << "Slough card found at index: " << index << endl;
	
}

// Test function for class FFNN of a specific neural network, using randomized inputs
// Displays the output and timing data
// Inputs are generated randomely each time the function is called
//
// Network 1 is the original "Blondie24" network
void testNetwork1()
{
	cout << " * * TESTING NETWORK 1 * * " << endl << endl;
	// *******TESTING NETWORK 1*******
	vector<int> network = { 4,32,40,10,1 };
	size_t num_nodes = 32 + 40 + 10 + 1;
	size_t num_weights = 1690;
	size_t num_inputs = 32;
	//vector<double> weights(num_weights, 0.1);


	// ****Testing member functions****
	FFNN net(network);

	// ****Timing Network 1****

	// Randomizing input
	//		Generating 1001 individually randomized input vectors
	size_t size = 1001; // For two test: 1000 evaluations, 1 evaluation
	double random;
	vector< vector<double> > input_vecs(size); // iterate through all input vectors
	for (size_t i = 0; i < size; i++)
	{
		vector<double> temp(num_inputs);
		for (auto it = temp.begin(); it != temp.end(); it++) // randomize input vector
		{
			random = ((double)(rand() % 21) - 10) / 10; // random number between 0 and 1, with accuracy to 0.1
			*it = random;
		}
		input_vecs[i] = temp;
	}

	// Timing 1000 evaluations
	auto t1 = std::chrono::high_resolution_clock::now(); // start timing

	for (auto it = input_vecs.begin(); it != input_vecs.end(); it++)
	{
		net.evaluate(&(*it));
	}

	auto t2 = std::chrono::high_resolution_clock::now(); // end timing

	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	cout << "1000 evaluations took: "
		<< time
		<< " nanoseconds\n";

	auto eval_sec = (1e+12) / time; // get eval per sec

	cout << "Evaluations per second: " << eval_sec << endl << endl;


	// Timing 1 evaluation
	t1 = std::chrono::high_resolution_clock::now(); // start timing

	double result = net.evaluate(&input_vecs[1000]); // use last randomized input vector

	t2 = std::chrono::high_resolution_clock::now(); // end timing
	time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	cout << "---->Output of single evaluation: " << result << endl;
	cout << "1 evaluation took: "
		<< time
		<< " nanoseconds\n";

	eval_sec = (1e+9) / time; // eval per sec

	cout << "Evaluations per second: " << eval_sec << endl;
}


// Test function for class FFNN of a specific neural network, using randomized inputs
// Displays the output and timing data
// Inputs are generated randomely each time the function is called
// 
// Network 2 is a large network with 10,000+ nodes
void testNetwork2()
{
	// *******TESTING NETWORK 2*******
	vector<int> network2 = { 6,100,5000,4000,1000,100, 1 };

	// get sizes of data
	size_t num_weights2 = network2[1] * network2[2] + network2[2] * network2[3] + network2[3] * network2[4]
		+ network2[4] * network2[5] + network2[5] * network2[6];
	size_t num_inputs2 = network2[1];
	size_t num_nodes = network2[1] + network2[2] + network2[3] + network2[4] + network2[5] + network2[6];

	// initialize weights
	//vector<double> weights2(num_weights2, 0.01);

	// generate random input
	vector<double> inputs2(num_inputs2);
	for (auto it = inputs2.begin(); it != inputs2.end(); it++)
	{
		double random = ((double)(rand() % 21) - 10) / 10; // random number between -1 and 1, with accuracy to 0.1
		*it = random;
	}

	// generate network
	cout << endl << " * * TESTING NETWORK 2 * * " << endl << endl;
	cout << "Number of nodes: " << num_nodes << endl;
	cout << "Size of weights: " << num_weights2 << endl;
	cout << "Size of input: " << num_inputs2 << endl;
	FFNN net2(network2);


	// ****Timing Network 2****

	auto t1 = std::chrono::high_resolution_clock::now(); // start timing

	double result = net2.evaluate(&inputs2); // use last randomized input vector

	auto t2 = std::chrono::high_resolution_clock::now(); // end timing

	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

	cout << endl << "---->Output of single evaluation: " << result << endl;
	cout << "1 evaluation took: "
		<< time
		<< " nanoseconds\n";

	auto eval_sec = (1e+9) / time; // eval per sec

	cout << "Evaluations per second: " << eval_sec << endl;
}

void test_ai_network()
{
	cout << endl << "****TESTING PLAYER_AI: MAKE INPUT" << endl << endl;
	Game g;
	Table t = g._t;
	State s = *t._state;
	Deck hand;
	Deck played;
	player_t me = S;
	player_t partner = N;


	//s._trump = hearts;
	s._dealer = partner;
	//s._bidder = me;
	//s._leader = E;
	//s._lead = spades;

	Card c(ten, clubs);
	s._cardup = c;
	s._up = c._suit;
	s._round2 = true;

	// Played cards:
	//Card lead(jack, spades);
	//played.addCard(lead);
	//Card second(nine, hearts);
	//played.addCard(second);

	// Cards in hand:
	hand.addCard(ace, diamonds);
	hand.addCard(nine, diamonds);
	hand.addCard(queen, diamonds);
	hand.addCard(jack, diamonds);
	hand.addCard(jack, hearts);

	FFNN_Evo net1("net1gen1");
	Player_AI ai(&s, me, &hand, &played, &net1);
	/*
	ai.processRound();
	ai._lead_card = lead;
	ai._top_card = lead;
	ai.processPlayed();
	*/


	cout << endl << endl << "----------------------------------------Testing Make Input:: " << endl << endl;


	vector<double> input = ai.makeInput(true, diamonds);
	cout << "Hand: " << endl;
	ai._hand->print_deck(1);

	cout << endl << "Input vector: " << endl;
	for (auto it = input.begin(); it != input.end(); it++)
	{
		cout << *it << " ";
	}
	cout << endl <<"Score: " << endl;
	for (size_t i = 0; i < 2; i++)
	{
		cout << input[i] << " ";
	}
	cout << endl << "Position: " << endl;
	for (size_t i = 2; i < 6; i++)
	{
		cout << input[i] << " ";
	}
	cout << endl << "Round: " << endl;
	for (size_t i = 6; i < 8; i++)
	{
		cout << input[i] << " ";
	}
	cout << endl << "Round 2 - next or opposite; " << endl;
	for (size_t i = 8; i < 10; i++)
	{
		cout << input[i] << " ";
	}
	cout << endl << "Card up: " << endl;
	for (size_t i = 10; i < 16; i++)
	{
		cout << input[i] << " ";
	}
	cout << endl << "Cards in hand: " << endl;
	for (size_t i = 16; i < 40; i++)
	{
		cout << input[i] << " ";
	}



	double result = ai.evaluate(&input);
	cout << endl << "Result of independent evaluation: " << result << endl;

	int decision = ai.bidDecide();
	cout << endl << "Bid decision: " << decision << endl;

} 

void test_memory()
{

	Game g;
	Table t = g._t;
	State s = *t._state;
	Deck hand;
	Deck played;
	player_t me = S;
	player_t partner = N;


	s._trump = clubs;
	s._dealer = me;
	s._bidder = me;
	s._leader = E;
	s._lead = spades;


	Card c(ten, clubs);
	s._cardup = c;
	s._up = c._suit;

	// Played cards:
	Card lead(jack, spades);
	played.addCard(lead);
	// Card second(nine, hearts);
	// played.addCard(second);

	// Cards in hand:
	hand.addCard(jack, hearts);
	hand.addCard(nine, hearts);
	hand.addCard(jack, clubs);
	hand.addCard(ten, diamonds);
	hand.addCard(ten, hearts);
	//hand.addCard(ten, clubs);

	for (int i = 0; i < 100; i++)
	{
		/*
		FFNN_Evo net1("net1gen1");
		//testEvolver();
		Player_AI ai(&s, me, &hand, &played, &net1);
		ai.processRound();
		ai._lead_card = lead;
		ai._top_card = lead;
		ai.processPlayed();
		ai.findLegal();
		int index = ai.getSlough();
		test_player();
		*/

		FFNN_Evo net("net1gen1");
		vector<FFNN_Evo*> netvec = { &net, &net, &net, &net };
		Game g2;
		g2.setNetworks(netvec);
		g2.newGame(0);
		if (i % 10 == 0) cout << "checkpoint: " << i << endl;
	}

}