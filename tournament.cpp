#include "tournament.h"

Tournament::Tournament(int num_nets) : _num_nets(num_nets)
{
	//_scores.assign(_num_nets, 0);
}

// create 100 new networks
//
// to be used only once at the beginning of training
void Tournament::initializeGen1()
{
	int gen = 1;

	int num_layers = 5;
	int num_input = 40;
	int num_weights1 = 100;
	int num_weights2 = 40;
	int num_weights3 = 10;
	_network = { num_layers, num_input, num_weights1, num_weights2, num_weights3, 1 };

	// Initial input weights
	vector<double> input_weights = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, // game state
		1.1, 1.2, 1.3, 1.4, 1.5, 1.6,								// card up 
		1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, // offsuit 1 and 2
		1.1, 1.2, 1.3, 1.4, 1.5,									// next suit
		2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7 };						// trump suit

	cout << "Initializing Generation 1" << endl;
	for (int i = 0; i < _num_nets; i++)
	{
		FFNN t(_network, input_weights); // network of random weights
		FFNN_Evo t2(t, 0.05); // convert to evolution network
		_nets.push_back(t2);
		string filename = "nets/gen1net" + std::to_string(i + 1);
		_nets[i].writeToFile(filename);
	}
	cout << "Generation 1 initialized" << endl;
}

// read generation of networks from file
void Tournament::readNets(int gen)
{
	cout << "Reading networks from files for generation " << gen;
	_nets.clear();
	string path = "nets/gen" + std::to_string(gen) + "net";
	for (int i = 0; i < _num_nets; i++) // read each network from file
	{
		string filename = path + std::to_string(i + 1);
		//cout << "Reading from file: " << filename << endl;
		FFNN_Evo t(filename); // create new network from file
		_nets.push_back(t); // add network to nets vector
	}
	cout << "Networks successfully read from files" << endl;
}

// play games for 1 generation
// each network plays num_games_per_net games
void Tournament::playGames(int num_games_per_net)
{
	cout << "Playing a round of " << num_games_per_net*_num_nets / 4 << " games" << endl;
	Game g;
	_scores.assign(_num_nets, 0);

	vector<int> indices;
	for (int i = 0; i < _num_nets; i++)
	{
		indices.push_back(i);
	}
	
	FFNN_Evo net1 = _nets[0];
	FFNN_Evo net2 = _nets[1];
	FFNN_Evo net3 = _nets[2];
	FFNN_Evo net4 = _nets[3];
	//_netvec = { net1, net2, net3, net4 };
	
	
	for (int i = 0; i < num_games_per_net; i++) 
	{
		if (i % 10 == 0) cout << "Playing round " << i + 1 << endl;
		std::random_shuffle(indices.begin(), indices.end()); // randomize order of networks
		
		for (int j = 0; j < _num_nets; j += 4) // each network plays one game with random players
		{
			int indexW = indices[j];
			int indexN = indices[j + 1];
			int indexE = indices[j + 2];
			int indexS = indices[j + 3];
			_netvec = { &_nets[indexW], &_nets[indexN], &_nets[indexE], &_nets[indexS] };
			
			g.setNetworks(_netvec);
			g.newGame(0);
			int scoreNS = g._prev_score_NS;
			int scoreWE = g._prev_score_WE;
			//cout << "Game Over: " << scoreNS << " to " << scoreWE << endl;
			if (scoreNS == 0 && scoreWE == 0) // hung game
			{
				//cout << "Hung game - no points awarded" << endl;
			}
			else if (scoreNS > scoreWE) // N-S win
			{
				_scores[indexN] += 15;
				_scores[indexS] += 15;
				_scores[indexW] += scoreWE;
				_scores[indexE] += scoreWE;
			}
			else // W-E win
			{
				_scores[indexW] += 15;
				_scores[indexE] += 15;
				_scores[indexN] += scoreNS;
				_scores[indexS] += scoreNS;
			}
		}
	}
	cout << "Finished round of games" << endl;
}

// generate next generation of networks
void Tournament::nextGen(int gen)
{
	int num_winners = _num_nets / 2;
	Evolver e(_num_weights);
	
	// sort networks by score
	vector<std::pair<int, int>> sortable_scores;
	for (int i = 0; i < _num_nets; i++)
	{
		std::pair<int, int> t(i, _scores[i]);
		sortable_scores.push_back(t);
	}
	std::sort(sortable_scores.begin(), sortable_scores.end(), [](auto &left, auto &right){
		return left.second < right.second; });

	// remove lowest 50 networks
	sortable_scores.erase(sortable_scores.begin(), sortable_scores.begin() + num_winners);

	// make new nets vector
	vector<FFNN_Evo> new_nets;
	for (int i = 0; i < num_winners; i++)
	{
		int index = sortable_scores[i].first;
		new_nets.push_back(_nets[index]);
		FFNN_Evo net2 = e.evolveNetwork(&_nets[index]);
		new_nets.push_back(net2);
	}
	_nets = new_nets;

	// write networks to files - once every 100 generations
	if (gen % 100 == 0)
	{
		string path = "nets/gen" + std::to_string(gen) + "net";
		for (int i = 0; i < _num_nets; i++)
		{
			string filename = path + std::to_string(i + 1);
			_nets[i].writeToFile(filename);
		}
		ofstream myfile;
		myfile.open("start_gen");
		if (myfile)	myfile << gen;
		myfile.close();
	}
}

void Tournament::run()
{
	int num_rounds = 100;
	int num_generations = 200;
	ifstream myfile;
	myfile.open("start_gen");
	if (myfile)
	{
		int start_gen;
		myfile >> start_gen;
		myfile.close();

		readNets(start_gen);
		int gen = start_gen;

		// loop through however many generations desired:
		for (int j = 0; j < num_generations; j++)
		{
			cout << "Beginning generation: " << gen << endl;
			playGames(num_rounds);
			
			for (int i = 0; i < _scores.size(); i++)
			{
				cout << "Network " << i + 1 << ": " << _scores[i] << endl;
			}
			

			cout << "End of generation " << gen << endl;
			cout << "Evolving next generation and writing to files..." << endl;
			gen++;
			nextGen(gen);
			
		}
		cout << "Finished running " << num_generations << " generations." << endl;
		
		
	}
	else
	{
		cout << "ERROR: Could not open start_gen file" << endl;
	}

	
}