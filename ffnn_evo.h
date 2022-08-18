// ffnn_evo.h
// Micah Smith

// header file for class FFNN_Evo
// an adapted version of FFNN that has the capacity to evolve following a normal distribution using the Evolver class

#ifndef _ffnn_evo_h_included_
#define _ffnn_evo_h_included_

#include "ffnn.h"


class FFNN_Evo: public FFNN {
public:
	FFNN_Evo(FFNN & a, double s = 0.05);
	FFNN_Evo(string filename);
	void writeToFile(string filename);
	void readFromFile(string filename);
	void initializeSigma(double s = 0.05);

// Sigma vector
	// number of sigma values = number of weights
	// NL1 + NL1*NL2 + NL2*NL3 + ...+ NL(N-1)*NLN
	vector<vector<double>> _sigma;

// Input Sigma vector
	// number of sigma values = number of inputs (size of _network[1])
	vector<double>_input_sigma;
};

#endif