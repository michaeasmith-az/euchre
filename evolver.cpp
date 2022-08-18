// evolver.cpp
// Micah Smith

// source file for class Evolver

#include "evolver.h"

Evolver::Evolver(int num_weights) : _num_weights(num_weights),_mt(_rd())
{

}

double Evolver::evolveSigma(double s)
{
	std::normal_distribution<double> rand_d(0, 1); // for normal distribution

	double tau = 1.0 / sqrt(2.0 * sqrt(_num_weights));
	double s2 = s * exp(tau*rand_d(_mt));
	return s2;
}

double Evolver::evolveWeight(double w, double s)
{

	std::normal_distribution<double> rand_d(0,1); // for normal distribution

	double w2 = w + s*rand_d(_mt);

	return w2;
}

FFNN_Evo Evolver::evolveNetwork(FFNN_Evo * ffnn)
{
	FFNN_Evo new_ffnn = *ffnn;
	for (size_t i = 0; i < ffnn->_input_weights.size(); i++)
	{
		double temp_s = ffnn->_input_sigma[i];
		new_ffnn._input_weights[i] = evolveWeight(ffnn->_input_weights[i], temp_s);
		new_ffnn._input_sigma[i] = evolveSigma(temp_s);
	}
	for (size_t i = 0; i < ffnn->_weights.size(); i++)
	{
		for (size_t j = 0; j < ffnn->_weights[i].size(); j++)
		{
			double temp_s = ffnn->_sigma[i][j];
			new_ffnn._weights[i][j] = evolveWeight(ffnn->_weights[i][j], temp_s);
			new_ffnn._sigma[i][j] = evolveSigma(temp_s);
		}
	}

	return new_ffnn;
}