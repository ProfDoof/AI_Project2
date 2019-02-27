//snowglobe.h has iostream and cmath
#include "snowglobe.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
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

// Random function can be done using the mersenne twister combined with random
// device and standard distribution

// population : 100
// elitism : keep x best (x = 20) (this will keep focus on depth (more generations))
// crossover : randomized
// fitnesss : value-based heuristic that also takes weight into account. WEIGHT LEFT

// how to deal with weight limit : initially generate good sets
//                                 if over limit kill


void outputKnapsack(vector<snowglobe> knapsack)
{
  int totalValue = 0;
  int totalWeight = 0;

  cout << "Take snowglobes ";

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

int main()
{
  //create the vector to hold all the snowglobes (and the other variables)
  vector<snowglobe> house;
  vector<snowglobe> knapsack;
  string filename;
  string line;
  ifstream fin;

  int numItems = 0;
  int maxWeight = 0;

  string snowglobeID;
  int snowglobeWeight = 0;
  int snowglobeValue = 0;

  //get the input file
  cout << "Enter filename: ";
  cin >> filename;

  //open the file and read in the snowglobes
  auto t1 = chrono::high_resolution_clock::now();
  fin.open(filename.c_str());

  if (fin.is_open())
  {
    //the first line has num items and max weight, so do that separately
    getline(fin, line);

    //replace the commas with spaces
    replace(line.begin(), line.end(), ',', ' ');

    //now we get the two numbers
    stringstream ss(line);
    ss >> numItems >> maxWeight;

    //loop through the rest of the file to get the snowglobes
    for (int i = 0; i < numItems; i++)
    {
      //read in the line and replace the commas
      getline(fin, line);
      replace(line.begin(), line.end(), ',', ' ');
      stringstream ssSnowglobe(line);

      //get the individual components and put them into a struct
      ssSnowglobe >> snowglobeID >> snowglobeWeight >> snowglobeValue;
      snowglobe current;
      current.id = snowglobeID;
      current.weight = snowglobeWeight;
      current.value = snowglobeValue;
      current.ratio = (double)(snowglobeValue) / snowglobeWeight;

      //put the snowglobe into the house
      house.push_back(current);
    }
  }
  else
  {
    cout << "Could not open file." << endl;
  }

  fin.close();
  auto t2 = chrono::high_resolution_clock::now();
  chrono::duration<double, std::milli> fp_ms = t2 - t1;

  cout << endl << "File Input time: " << fp_ms.count() << endl;

  //Exhaustive Search implemented here.
  t1 = chrono::high_resolution_clock::now();
  knapsack = exhaustive(house, maxWeight, numItems);
  t2 = chrono::high_resolution_clock::now();
  fp_ms = t2 - t1;
  cout << "By Exhaustive Search: ";
  outputKnapsack(knapsack);
  cout << endl;
  cout << "Exhaustive Search time: " << fp_ms.count() << endl;
}
