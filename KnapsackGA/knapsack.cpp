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
  int currentWeight = 0;

	// iterate through gene, only flip random bit if the set's weight is less than the carry cap
	for (int i = 0; i < gene.size() && currentWeight <= maxWeight; i++) {

    currentWeight += items[i].weight;
    if (dist(engine) < (2.0 / gene.size()) && currentWeight <= maxWeight ) {
			gene[i] = true;
    }

	}
	return gene;
}

// generates (size) chromosomes (randomly) and appends them to the population
void generatePopulation(const int size, const vector<Item> items, const int maxWeight, vector<Chromosome> &population) {

	// i : nth chromosome being generated
	for (int i = 0; i < size; i++) {
			vector<bool> gene = randGene(items, maxWeight);
			Chromosome chromo(items, gene, maxWeight);
    	population.push_back(chromo);
	}
}

int main() {
  // Declare our population size, and the number of minutes we want the
  // GA to run. Then we multiply the number of minutes by 60000 that way
  // we only change a single number.
	// int genLimit = 20000;
  int popSize = 100;
	int maxWeight = 0;
  float minutes = 3;
  minutes *= 60000;
	vector<Item> items;

	// READ FILE //
	string filename;
	cout << "Enter a filename: ";
	cin >> filename;
	readFile(filename, items, maxWeight);

	// INITIALIZE POPULATION //
	vector<Chromosome> population;
	generatePopulation(popSize, items, maxWeight, population);
  // Need the initial fitness calculation.


	// BEGIN GENERATIONS //
  int gen = 1;

  // maxFit : best fitness in current population
  // minFit : worst fitness in current population
  // bestValue : best solution value
  // bestChromo: The best chromosome we have found.
  int maxFit = 0;
  int minFit = 0;
	int bestValue = 0;
  Chromosome bestChromo(items, vector<bool>(items.size(), 0), maxWeight);

  // Start the timer so that we only run for as many minutes as defined in the
  // minutes variable.
  auto startTime = chrono::high_resolution_clock::now();
  auto currentTime = startTime;
  chrono::duration<double, std::milli> compare = currentTime - startTime;

  while (compare.count() < minutes) {
    vector<Chromosome> intermediatePopulation;
		vector<Chromosome> newGeneration;

    // maxfit holds best solution even if invalid chromosomes are allowed
    // The best chromo is also within the correct weight because the fitness function
    // automatically drops the
    // sort population based on FITNESS and if FITNESS is equal it is based off of
    // value. The sort is based off of the Chromosome struct.
    sort(population.begin(), population.end());
    minFit = population.begin()->fitness;
    maxFit = population.back().fitness;


		// ERROR: if fitness is not based on value alone then the back of population may not be the best solution
    // HERE FIND BEST VALUE ?? //
    // for (auto i = population.begin(); i != population.end(); i++) {
    //   if (i->value > bestValue && i->weight <= maxWeight) {
    //     bestChromo.gene = i->gene;
    //     bestChromo.value = i->value;
    //     bestChromo.weight = i->weight;
    //     bestValue = i->value;
    //     cout << "Gen: " << gen << "\tNew Best: " << i->weight << " " <<  i->value << endl;
    //
    //   }
    // }

		if (population.back().value > bestChromo.value) {
			cout << "Gen: " << gen << "\tNew Best: " << population.back().weight << " " << population.back().value << endl;
      bestChromo.gene = population.back().gene;
      bestChromo.value = population.back().value;
      bestChromo.weight = population.back().weight;
		}

    // Normalize the fitness
    for (auto it = population.begin(); it != population.end(); it++)
    {
      it->fitnessSet(bestChromo.value, maxWeight);
    }

		// POPULATE INTERMEDIATE POPULATION //
		// if the state valid and cleared by percentage then push it into inter pop
		for (int i = 0; intermediatePopulation.size() < popSize; i++, i %= popSize) {
      if (dist(engine) <= population[i].fitness) {
        Chromosome temp(items, population[i].gene, maxWeight);
        intermediatePopulation.push_back(temp);
      }
		}

		while (!intermediatePopulation.empty()) {
      // cout << "1" << endl;
			// DIRECT COPY //
			// directly pushes state straight to new gen 25% of the time
      // if it is the last one any crossover generated would be equal to itself
      // plus that would take us over the population limit so just push that
      // sucker in.
			if (dist(engine) <= 0.25 || intermediatePopulation.size() == 1) {
        // Mutate
        for (int i = 0; i < intermediatePopulation[0].gene.size(); i++)
        {
          if (dist(engine) <= .005) {
            intermediatePopulation[0].gene[i] = !intermediatePopulation[0].gene[i];
          }
        }
        // Add to new population
				newGeneration.push_back(intermediatePopulation[0]);
        // Delete from intermediate population
        intermediatePopulation.erase(intermediatePopulation.begin());
      }

			// SINGLE POINT CROSSOVER //
			// pushes both crossover's and then deletes both chromosomes from
      // the intermediate population.
			else {
				vector<bool> crossed1;
        vector<bool> crossed2;
				int crossPoint = dist(engine)*items.size();
        int bPoint = dist(engine)*intermediatePopulation.size();

        Chromosome chromoA = intermediatePopulation[0];
        Chromosome chromoB = intermediatePopulation.at(bPoint);

        for (int i = 0; i < crossPoint; i++ )
        {
          crossed1.push_back(chromoA.gene[i]);
          crossed2.push_back(chromoB.gene[i]);
        }

        for (int i = crossPoint; i < items.size(); i++)
        {
          crossed1.push_back(chromoB.gene[i]);
          crossed2.push_back(chromoA.gene[i]);
        }

        intermediatePopulation.erase(intermediatePopulation.begin()+bPoint);
        intermediatePopulation.erase(intermediatePopulation.begin());

				// if the crossed is valid push to new gen
				// if the crossed is not valid push random chromo
        // randomly passs through invalid chromo occasionally .5% chance
        // vector<bool> gene = valid(items, crossed, maxWeight) ? crossed : randGene(items, maxWeight);

        // Mutate function
        for (int i = 0; i < crossed1.size(); i++)
        {
          if (dist(engine) <= .005) {
            crossed1[i] = !crossed1[i];
          }

          if (dist(engine) <= .005) {
            crossed2[i] = !crossed2[i];
          }
        }

				Chromosome chromo1(items, crossed1, maxWeight);
        Chromosome chromo2(items, crossed2, maxWeight);
				newGeneration.push_back(chromo1);
        newGeneration.push_back(chromo2);
      }
		}

		// make what was old new
    // figure out why it doesn't like population = newGeneration
		population.clear();
		copy(newGeneration.begin(), newGeneration.end(), back_inserter(population));
		gen++;
    currentTime = chrono::high_resolution_clock::now();
    compare = currentTime - startTime;
	}

	cout << "Final Answer: " << bestChromo.value << endl;
}
