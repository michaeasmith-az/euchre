// ffnn.h
// Micah Smith
// updated 08/18/2022

// header file for class FFNN


#ifndef _ffnn_h_included_
#define _ffnn_h_included_

#include<iostream>
#include<fstream>
using std::ofstream;
using std::ifstream;
using std::cout;
using std::endl;
#include<cstdlib> // for size_t
using std::size_t;
#include<vector>
using std::vector;
#include<string>
using std::string;
#include<math.h> // for exponent
#include <stdlib.h> // for rand
#include <random>
#include <time.h> // for srand(time(NULL))
#include <chrono> // for timing

/*
FFNN
Feed-forward neural network
	Generates a feed-forward neural network, either from a vector storing the number of layers and the number of nodes in each layer,
	or from a file.
	The weights can be given as input or generated randomely.
	The output of the network can be evaluated for an input vector (representing a game board or other data).

Invariants: 
	--- The size of _weights is given by the formula: NL1 + NL1*NL2 + NL2*NL3 + ...+ NL(N-1)*NLN,
		where NLi = _network[i]
	--- _network[0] is the number of layers in the network, so the size of _network is _network[0] + 1
	--- The size of _nodes = _network[0], the number of layers in the network
	--- the size of _nodes[i] = _network[i+1], the number of nodes in the corresponding network i+1
*/
class FFNN {
public:

	FFNN();
	FFNN(string filename); // construct from file
	FFNN(vector<int> network); // construct from network vector; randomize weights
	FFNN(vector<int> network, vector<double> input_weights);
	FFNN(vector<int> & network, const vector<vector<double>> & weights); // construct from network and weights vectors
	FFNN(vector<int> & network, vector<double> input_weights, const vector<vector<double>> & weights); // construct from network and weights vectors
	void allocateNodes();
	void writeToFile(string filename);
	void readFromFile(string filename);
	void randomizeWeights(double min_max = 0.2);
	double sigmoid(double x, double a = 1, double b = 1, double c = 0);
	double evaluate(vector<double> * inputs); // Board Evaluation Function

// Network vector
	// _network[0] : number of layers = N
	// _network[1] : first layer (input) = NL1
	// _network[2] : second layer = NL2
	//			...
	// _netowrk[n] : last layer (output) = NLN
	vector<int> _network;

// Nodes vector
	// Each element is a vector of nodes
	// The value of each node corresponds to its output
	vector< vector<double> > _nodes;

// Weights vector
	// number of weights =
	// NL1 + NL1*NL2 + NL2*NL3 + ...+ NL(N-1)*NLN
	vector<vector<double>> _weights;

// Input weights
	// Separate from weights vector
	// Evaluation does not use sigmoid function

	vector<double> _input_weights; 
};

#endif
