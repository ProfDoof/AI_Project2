//snowglobe.h has iostream and cmath
#include "Item.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>
using namespace std;


// THINGS WE NEED
// Crossover method
// Mutation Rate
// Evaluation (FITNESS)
//   Knapsack
//    Highest value
//    Remaining capacity
//    ? Number of items
//    ? Best Ratio
//   TSP
//    Distance
//   Substitution Cipher
//    Unigraph/Digraph/Trigraph/Vowel
// Copy Rate
// Population Rate
// How to handle crowding (I AM A FAN OF CATASTROPHIC MUTATION)

// What we need to ask HOMER
//  How to determine if we are crowding
//    String or Boolean Array
//  How to deal with weight limit OR how to generate initial sets
//  Dealing with crossover for something like knapsack.

// Random function can be done using the mersenne twister combined with random
// device and standard distribution

// population : 100
// elitism : keep x best (x = 20) (this will keep focus on depth (more generations))
// crossover : randomized
// fitnesss : value-based heuristic that also takes weight into account. WEIGHT LEFT

// how to deal with weight limit : initially generate good sets
//                                 if over limit kill


void readFile(const string filename, vector<Item> &items, int &maxWeight) {

  string line;
  string itemID;
  int numItems;
  int itemWeight;
  int itemValue;

  ifstream fin;
  fin.open("..//Test_files/" + filename);

  if (fin.is_open()) {
 
    // first line in file constains : "num_of_items, max_weight"
    // read in first line of file and assign values accordingly (ignore commas)
    getline(fin, line);
    replace(line.begin(), line.end(), ',', ' ');
    stringstream ss(line);
    ss >> numItems >> maxWeight;

    // loop through the rest of the line in the file (populating items vector)
    for (int i = 0; i < numItems; i++) {

      // read in the line (ignore commas)
      getline(fin, line);
      replace(line.begin(), line.end(), ',', ' ');
      stringstream ss(line);

      // line in file : "id, weight, value"
      // populate an item with line data
      ss >> itemID >> itemWeight >> itemValue;
      Item current;
      current.id = itemID;
      current.weight = itemWeight;
      current.value = itemValue;
      current.ratio = (double)(itemValue) / itemWeight;

      //put the snowglobe into the house
      items.push_back(current);
    }
  }
  else {
    cout << "Could not open file." << endl;
    exit(0);
  }
  fin.close();
}

void outputKnapsack(vector<Item> knapsack) {
  int totalValue = 0;
  int totalWeight = 0;

  cout << "Take items ";

  for (int i = 0; i < knapsack.size(); i++)
  {
    if (totalValue == 0)
    {
      cout << knapsack[i].id;
    }
    else
    {
      cout << ", " << knapsack[i].id;
    }

    totalWeight += knapsack[i].weight;
    totalValue += knapsack[i].value;
  }

  cout << " for a total weight of " << totalWeight << " and a value of " << totalValue << "." << endl;
}





// return : value of a state
int getValue(vector<Item> items, vector<bool> state) {
    int value = 0;
    for (auto it = state.begin(); it != state.end(); it++)
        if (*it)
            value += items.at(it - state.begin()).value;
    return value;
}

// return : the weight of a state
int getWeight(vector<Item> items, vector<bool> state) {
    int weight = 0;
    for (auto it = state.begin(); it != state.end(); it++)
        if (*it)
            weight += items.at(it - state.begin()).weight;
    return weight;
}

// prints out the contents of a chromosome
void printChromo(vector<bool> state) {
	for_each(state.begin(), state.end(), [](auto it) { cout << it << " "; }); cout << endl;
}

void printPopulation(vector<vector<bool>> pop) {	
	for_each(pop.begin(), pop.end(), [](auto it) { printChromo(it); }); cout << endl;
}

// (P) size : the number of chromosomes to be generated
// generates (size) chromosomes (random) and appends them to the population
void generatePopulation(const int size, const vector<Item> items, const int maxWeight, vector<vector<bool>> &population) {

	// i : nth chromosome being generated
	for (int i = 0; i < size; i++) {
		vector<bool> chromo(items.size(), false);

		// iterate through chromo, only flip random bit if the set's weight is less than the carry cap
		for (auto it = chromo.begin(); it != chromo.end(); it++) {
			if (rand() % 2)
				*it = true;

			if (getWeight(items, chromo) > maxWeight) {
				*it = false;
				break; // yes, I did the unspeakable mwahahahaaha
			}
		}
    	population.push_back(chromo);
	}
}

int main() {
	int popSize = 10;
	int maxWeight = 0;
	vector<Item> items;

	// READ FILE //
	string filename;
	cout << "Enter a file: ";
	cin >> filename;
	readFile(filename, items, maxWeight);
	outputKnapsack(items);

	// INITIALIZE POPULATION //
	vector<vector<bool>> population;
	generatePopulation(popSize, items, maxWeight, population);
	printPopulation(population);

	// LOOP //

	// BREED //

	// MUTATION //

	// FITNESS //

	// ELITISM //

	// END LOOP //



}
