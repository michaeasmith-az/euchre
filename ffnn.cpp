//ffnn.cpp
// Micah Smith

// source file for class FFNN

#include "ffnn.h"


//****FFNN Constructors

FFNN::FFNN()
{

}

// Pre: filename is the name of a readable file with a feed forward neural network stored in the following format:
//		N NL1 NL2 ... NLN w_0 w_1 w_2 ... w_n
//		where N is the number of layers, NLi is the number of nodes in layer i, and w_j is weight number j, 
//		and each element is separated only by whitespace
// Post: If the file is opened and read successfully, _network, _nodes, and _weight are initialized to match the 
//		neural network stored in the file. Otherwise, an error message is displayed.
FFNN::FFNN(string filename)
{
	readFromFile(filename);
}

// Pre: network is a vector of the format {N, NL1, NL2, ..., NLN}
//		where N is the number of layers in a feedforward neural network, and NLi is the number of nodes in layer i
// Post: _network = network, the values of _weights are initialized to random values between -1 and 1, and space is allocated for _nodes
FFNN::FFNN(vector<int> network): _network(network)
{
	_input_weights.assign(_network[1], 1); // initialzie input weights to 1
	randomizeWeights();
	allocateNodes();
}

// Pre: network is a vector of the format {N, NL1, NL2, ..., NLN}
//		where N is the number of layers in a feedforward neural network, and NLi is the number of nodes in layer i
// Post: _network = network, _input_weights = input_weights, the values of _weights are initialized to random values between -1 and 1, 
// and space is allocated for _nodes
FFNN::FFNN(vector<int> network, vector<double> input_weights): _network(network), _input_weights(input_weights)
{
	randomizeWeights();
	allocateNodes();
}

// Pre: network is a vector of the format {N, NL1, NL2, ..., NLN}
//		where N is the number of layers in a feedforward neural network, and NLi is the number of nodes in layer i
//		weights is a vector of weights corresponding to the neural network
// Post: _network = network, _weights = weights, space is allocated for _nodes, 
// _input_weights is resized to the size of the input layer, and its elements are initialized to 1
FFNN::FFNN(vector<int> & network, const vector<vector<double>> & weights) : _network(network)
{
	_input_weights.assign(_network[1], 1); // initialzie input weights to 1
	_weights = weights;
	allocateNodes();
}

// Pre: network is a vector of the format {N, NL1, NL2, ..., NLN}
//		where N is the number of layers in a feedforward neural network, and NLi is the number of nodes in layer i
//		weights is a vector of weights corresponding to the neural network
// Post: _network = network, _input_weights = input_weights, _weights = weights, and space is allocated for _nodes
FFNN::FFNN(vector<int> & network, vector<double> input_weights, const vector<vector<double>> & weights): _network(network), _input_weights(input_weights)
{
	_weights = weights;
	allocateNodes();
}

// **** Member functions

// allocateNodes
// Resizes _nodes to the appropriate size to store the nodes of the network
void FFNN::allocateNodes()
{
	size_t num_layers = _network[0];
	vector<vector<double>> temp(num_layers);
	_nodes = temp;

	// allocate space for nodes, and initialize to 0
	for (size_t i = 0; i < num_layers; i++) // iterate through each layer of nodes
	{
		vector<double> temp2(_network[i + 1], 0);
		_nodes[i] = temp2;
	}
}

// writeToFile
// Writes the neural network to a file in a format which can be read by FFNN
void FFNN::writeToFile(string filename)
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
		myfile.close();

	}
	else cout << "ERROR: File could not be opened for writing" << endl;

}

// readFromFile
// Reads a neural network from a file and sets _network and _weights to match the network
void FFNN::readFromFile(string filename)
{
	_network.clear();
	_nodes.clear();
	_input_weights.clear();
	_weights.clear();
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

	double temp_double = 0;
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
	myfile.close();
	allocateNodes();
	
}


// randomizeWeights
// sets all weights to random values within += min_max
void FFNN::randomizeWeights(double min_max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> rand_d(-min_max, min_max); // for uniform random distribution

	_weights.clear();


	_weights.resize(_network[0] - 1); // _weights[0] = weights for layer 2 (input layer has no inputs)
	if (_network[0] > 1) // more than one network layer
	{
		size_t layer_size = 0;
		double temp_double;
		for (size_t i = 0; i < _weights.size(); i++) // loop through layers of weights
		{
			layer_size = _network[i+1] * _network[i + 2];
			_weights[i].resize(layer_size); // resize weights vector
		}
	
		for (auto it = _weights.begin(); it != _weights.end(); it++)
		{
			for (auto it2 = it->begin(); it2 != it->end(); it2++)
			{
				double rand_weight = rand_d(mt); // uniform random value between -min_max and +min_max
				*it2 = rand_weight;
			}
		}
	}
}

// sigmoid
// Sigmoid function used by the function evaluate()
double FFNN::sigmoid(double x, double a, double b, double c)
{
	double y = a / (1 + exp(-x / b)) + c;
	//int y = x;
	return y;
}

// evaluate
// "Board Evaluation Function" - evaluates the output of the network for a given input vector
// Pre: Size of inputs = size of _network[1]
// Post: The neural network output for the given input is returned
double FFNN::evaluate(vector<double> * inputs)
{
	size_t num_layers = _network[0];
	if (num_layers == 0) return 0; // trivial: network with zero nodes

	size_t num_inputs = _network[1]; // number of inputs

	// set all nodes to 0
	for (size_t i = 0; i < num_layers; i++) // iterate through each layer of nodes
	{
		std::fill(_nodes[i].begin(), _nodes[i].end(), 0);
	}


	// calculate first layer outputs
	for (size_t i = 0; i < num_inputs; i++)
	{
		_nodes[0][i] = (*inputs)[i] * _input_weights[i];
	}

	// calculate outputs for other layers
	//double val = 0;
	size_t layer_count = 1;
	size_t prev_layer_count = layer_count - 1;
	size_t weight_layer_count = 0;
	//size_t weights_size = _weights.size();
	
	while (layer_count < num_layers)
	{
		
		// weight_count = # of weights counted so far in this layer

		size_t weight_count = 0;
		size_t prev_layer_size = _network[layer_count];
		size_t curr_layer_size = _network[layer_count + 1];
		vector<double> previous_layer = _nodes[prev_layer_count];
		

		
		//cout << "Current layer: " << layer_count + 1 << endl;
		
		// iterate through each node of the previous layer
		for (size_t prev_layer_node_count = 0; prev_layer_node_count < prev_layer_size; prev_layer_node_count++) 
		{ 
			double val = 0;
			//cout << "----Previous layer node: " << prev_layer_node_count + 1 << endl;
			//cout << "------------Node value: " << previous_layer[prev_layer_node_count] << endl;
			
			// iterate through each weight of the node (one for each node on the current layer)
			for (size_t node_count = 0; node_count < curr_layer_size; node_count++) 
			{
				//cout << "--Current layer node: " << node_count + 1 << endl;
				size_t weight_num = weight_count + node_count;
				//cout << "------Weight: " <<  weight_num + 1 << endl;
				//cout << "------------Weight value: " << _weights[prev_layer_count][weight_num] << endl;
				//val += previous_layer[prev_layer_node_count] * _weights[prev_layer_count][weight_num];
				//double increment = previous_layer[prev_layer_node_count] * _weights[prev_layer_count][weight_num];
				_nodes[layer_count][node_count] += previous_layer[prev_layer_node_count] * _weights[prev_layer_count][weight_num];
				//val = _nodes[layer_count][node_count];
				//cout << "------------Increment: " << increment << endl;
				//cout << "------------Value: " << val << endl;
			}
			weight_count += curr_layer_size;

			//_nodes[layer_count][node_count] = val;//sigmoid(val); // calculate the output of the node
		}

		for (size_t i = 0; i < curr_layer_size; i++)
		{
			_nodes[layer_count][i] = sigmoid(_nodes[layer_count][i]);
		}
		
		layer_count++;
		prev_layer_count++;
		weight_layer_count++;
	}

	//return _nodes[num_layers - 1].back(); // output is the value of the last node in the network
	return _nodes[num_layers - 1][0];
}


/*
CLASS NOTES:

POTENTIALLY BETTER PERFORMANCE MODEL:

for (unsigned int i=1; i<layer_.size(); ++i)
	{
	for (unsigned int j=0; j<layer_[i].size(); ++j)
	{
		double temp=0.0;
		for (unsigned int k=0; k<layer_[i-1].size(); ++k)
		{
			temp+=layer_[i-1][k].value*(layer_[i][j].weights[k]);
		}
	layer_[i][j].value=sigmoid(temp);
	}
			
*/