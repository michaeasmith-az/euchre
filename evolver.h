// evolver.h
// Micah Smith
// updated 08/18/2022

// header file for class Evolver

// used to evolve a neural network FFNN_Evo using a normal distribution

#ifndef _evolver_h_included_
#define _evolver_h_indluded_
#include "ffnn_evo.h"

// Evolver
class Evolver {
public:
	Evolver(int num_weights);
	double evolveWeight(double w, double s);
	double evolveSigma(double s);
	FFNN_Evo evolveNetwork(FFNN_Evo * ffnn);

	int _num_weights;
	std::random_device _rd; // random seed
	std::mt19937 _mt;
};


#endif
