// ffnn_evo.cpp
// Micah Smith

// source file for class FFNN_Evo
#include "ffnn_evo.h"

FFNN_Evo::FFNN_Evo(FFNN & a, double s)
{
	_network = a._network;
	_nodes = a._nodes;
	_input_weights = a._input_weights;
	_weights = a._weights;
	_sigma.resize(a._weights.size());
	initializeSigma(s);
}

FFNN_Evo::FFNN_Evo(string filename)
{
	readFromFile(filename);
}

void FFNN_Evo::initializeSigma(double s)
{
	
	int input_size = _network[1];
	_input_sigma.assign(input_size, s);

	for (size_t i = 0; i < _weights.size(); i++)
	{
		int size = _weights[i].size();
		_sigma[i].assign(size, s);
	}
}

void FFNN_Evo::writeToFile(string filename)
{
	// open file for writing
	ofstream myfile;
	myfile.open(filename);
	if (myfile.is_open())
	{
		// write layers to file
		for (auto it = _network.begin(); it != _network.end(); it++)
		{
			myfile << *it << endl;
		}

		// write input weights to file
		for (auto it = _input_weights.begin(); it != _input_weights.end(); it++)
		{
			myfile << *it << " ";
		}

		myfile << endl;

		//write weights to file
		for (auto it = _weights.begin(); it != _weights.end(); it++)
		{
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				myfile << *it2 << " ";
			}
		}
		myfile << endl;

		//write sigma values to file
		for (auto it = _sigma.begin(); it != _sigma.end(); it++)
		{
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				myfile << *it2 << " ";
			}
		}
		myfile << endl;

		// write input sigma values to file
		for (auto it = _input_sigma.begin(); it != _input_sigma.end(); it++)
		{
			myfile << *it << " ";
		}
		myfile.close();
	}
	else cout << "ERROR: File could not be opened for writing" << endl;
}

// readFromFile
// Reads a neural network from a file and sets _network and _weights to match the network
void FFNN_Evo::readFromFile(string filename)
{
	_network.clear();
	_nodes.clear();
	_input_weights.clear();
	_weights.clear();
	_sigma.clear();
	_input_sigma.clear();
	ifstream myfile;
	myfile.open(filename);

	// read network size from file
	int num_layers;
	myfile >> num_layers;
	_network.push_back(num_layers);

	// read network layers from file
	int temp;
	for (size_t i = 1; i < num_layers + 1; i++)
	{
		myfile >> temp;
		_network.push_back(temp);
	}

	// resize input weights vector
	_input_weights.resize(_network[1]);

	double temp_double;
	// read input weights from file
	for (size_t i = 0; i < _network[1]; i++)
	{
		myfile >> temp_double;
		_input_weights[i] = temp_double;
	}

	// resize weights vector
	_weights.resize(num_layers);

	// read weights from file
	size_t weight_layer_size = 0;
	for (size_t i = 0; i < num_layers - 1; i++)
	{
		size_t prev_layer_size = _network[i + 1];
		size_t curr_layer_size = _network[i + 2];
		weight_layer_size = prev_layer_size * curr_layer_size;
		_weights[i].resize(weight_layer_size); // resize weights vector
		for (size_t j = 0; j < weight_layer_size; j++)
		{
			myfile >> temp_double;
			_weights[i][j] = temp_double;
		}
	}

	/*
	for (size_t i = 0; i < _weights.size(); i++)
	{
	for (size_t j = 0; j < _weights[i].size(); j++)
	{
	cout << "Weight " << i << " , " << j << " : " << _weights[i][j] << endl;
	}
	}
	*/
	
	// read sigmas from file
	_sigma.resize(num_layers);
	size_t sigma_layer_size = 0;
	temp_double = 0;
	for (size_t i = 0; i < num_layers - 1; i++)
	{
		size_t prev_layer_size = _network[i + 1];
		size_t curr_layer_size = _network[i + 2];
		sigma_layer_size = prev_layer_size * curr_layer_size;
		_sigma[i].resize(sigma_layer_size); // resize weights vector
		for (size_t j = 0; j < sigma_layer_size; j++)
		{
			myfile >> temp_double;
			_sigma[i][j] = temp_double;
		}
	}

	// resize input weights vector
	_input_sigma.resize(_network[1]);

	// read input weights from file
	for (size_t i = 0; i < _network[1]; i++)
	{
		myfile >> temp_double;
		_input_sigma[i] = temp_double;
	}
	
	myfile.close();
	allocateNodes();
	
}