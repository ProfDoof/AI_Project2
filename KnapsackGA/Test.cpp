#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include "Chromosome.h"
using namespace std;

// Generate random seed and other random tools
random_device rd{};
mt19937 engine{rd()};
uniform_real_distribution<double> dist{0.0, 1.0};

void readFile(const string filename, vector<Item> &items, int &maxWeight) {

  string line;
  string itemID;
  int numItems;
  int itemWeight;
  int itemValue;

  ifstream fin;
  fin.open("../Test_Files/" + filename);

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

// return : the weight of a state
int getWeight(vector<Item> items, vector<bool> state) {
  int weight = 0;
  for (auto it = state.begin(); it != state.end(); it++)
    if (*it)
      weight += items.at(it - state.begin()).weight;
  return weight;
}

void printPopulation(vector<Chromosome> pop) {
	/*for (auto it = pop.begin(); it != pop.end(); it++) {
		cout << *it << endl;
	}*/
	double fit = 0;
	for (auto it = pop.begin(); it != pop.end(); it++) {
		fit += it->fitness;
	}

	cout << fit / pop.size() << endl;

}

// return : if a state is valid (e.g. if it is within the carrying capactiy)
bool valid(vector<Item> items, vector<bool> state, int maxWeight) {
	int weight = 0;
  for (auto it = state.begin(); it != state.end(); it++)
    if (*it)
      weight += items.at(it - state.begin()).weight;
  return weight <= maxWeight;
}

// return : randomly generated gene (within carrying capacity)
vector<bool> randGene(vector<Item> items, int maxWeight) {
	vector<bool> gene(items.size(), false);

  // Change condition, also get rid of break in FOR LOOP.
	// iterate through gene, only flip random bit if the set's weight is less than the carry cap
	for (auto it = gene.begin(); it != gene.end(); it++) {
		if (dist(engine) < ( 1.0 / items.size()))
        if (rand() % 2)
			*it = true;

		if (getWeight(items, gene) >= maxWeight) {
			*it = false;
			break; // yes, I did the unspeakable mwahahahaaha
		}
	}
	return gene;
}

// generates (size) chromosomes (randomly) and appends them to the population
void generatePopulation(const int size, const vector<Item> items, const int maxWeight, vector<Chromosome> &population) {

	// i : nth chromosome being generated
	for (int i = 0; i < size; i++) {
			vector<bool> gene = randGene(items, maxWeight);
			Chromosome chromo(items, gene);
            cout << chromo << endl;
    	population.push_back(chromo);
	}
}

int main() {
  // Fix random function
  // Use time

	int genLimit = 2000;
	int popSize = 100;
	int maxWeight = 0;
	vector<Item> items;

	// READ FILE //
	string filename;
	cout << "Enter a filename: ";
	cin >> filename;
	readFile(filename, items, maxWeight);

	// INITIALIZE POPULATION //
	vector<Chromosome> population;
	generatePopulation(popSize, items, maxWeight, population);


/*
	// BEGIN GENERATIONS //
  int gen = 1;

  // maxFit : best fitness in current population
  // minFit : worst fitness in current population
  // bestValue : best solution value
  int maxFit = 0;
  int minFit = 0;
	int bestValue = 0;

  while (gen < genLimit) {
    vector<Chromosome> intermediatePopulation;
		vector<Chromosome> newGeneration;

    // maxfit holds best solution if no invalid chromosomes are allowed
    // sort population based on FITNESS. The sort is based off of the Chromosome struct.
    sort(population.begin(), population.end());
    minFit = population.begin()->fitness;
    maxFit = population.back().fitness;

		// ERROR: if fitness is not based on value alone then the back of population may not be the best solution
    // HERE FIND BEST VALUE ?? //
		if (population.back().value > bestValue) {
			cout << "Gen: " << gen << "\tNew Best: " << population.back().weight << " " << population.back().value << endl;
			bestValue = population.back().value;
		}

		// POPULATE INTERMEDIATE POPULATION //
		// if the state valid and cleared by percentage then push it into inter pop
		for (int i = 0; intermediatePopulation.size() < popSize; i++, i %= popSize) {
			if (population.at(i).weight <= maxWeight)
      // NORMALIZE FITNESS //
				if (rand() / (double(RAND_MAX) + 1.0) <= (((1.0 * (population.at(i).fitness - minFit))  / ((maxFit - minFit) +.1))  * (.95 - .1) + .1))
					intermediatePopulation.push_back(population.at(i));
		}

		for (auto it = intermediatePopulation.begin(); it != intermediatePopulation.end(); it++) {
			// DIRECT COPY //
			// directly pushes state straight to new gen 25% of the time
			if (dist(engine) <= 0.25))
				newGeneration.push_back(*it);

			// SINGLE POINT CROSSOVER //
			// pushes crossover if it is valid, otherwise push a randomly genereated state
			else {
				vector<bool> crossed;
				int crossPoint = dist(engine)*items.size();

				Chromosome chromoA = *it;
				Chromosome chromoB = intermediatePopulation.at(int(dist(engine)*popSize));

				crossed.insert(crossed.begin(), chromoA.gene.begin(), chromoA.gene.begin() + crossPoint);
				crossed.insert(crossed.begin() + crossPoint, chromoB.gene.begin() + crossPoint, chromoB.gene.end());

				// if the crossed is valid push to new gen
				// if the crossed is not valid push random chromo
        // randomly passs through invalid chromo occasionally .5% chance
        vector<bool> gene = valid(items, crossed, maxWeight) ? crossed : randGene(items, maxWeight);

				Chromosome chromo(items, gene);
				newGeneration.push_back(chromo);
			}
		}

		// MUTATAION // ERROR MUTATION CAN GENERATE SOLUTION OVER WEIGHT LIMIT
		for (auto it = newGeneration.begin(); it != newGeneration.end(); it++) {
			if ((rand() / (double(RAND_MAX) + 1.0) <= 0.005))
				it->mutate(rand() % items.size());
		}

		// make what was old new
    // figure out why it doesn't like population = newGeneration
		population.clear();
		copy(newGeneration.begin(), newGeneration.end(), back_inserter(population));
		gen++;
	}
	cout << "Final Answer: " << bestValue << endl;*/
}
