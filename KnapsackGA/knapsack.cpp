//snowglobe.h has iostream and cmath
#include "snowglobe.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

void iterateArray(bool* array, int houseSize)
{
  for (int i = 0; i < houseSize; i++)
  {
    //cout << "iterateArray i = " << i << endl;
    if (array[i])
    {
      array[i] = false;
    }
    else
    {
      array[i] = true;
      return;
    }
  }
}

vector<snowglobe> exhaustive(vector<snowglobe> house, int maxWeight, int houseSize)
{
  //variables
  vector<snowglobe> bestKnapsack;
  int bestValue = 0;

  bool* array = new bool[houseSize];

  for (int i = 0; i < houseSize; i++)
  {
    array[i] = false;
  }

  for (int i = 0; i < pow(2, houseSize); i++)
  {
    //cout << "i = " << i << endl;
    //make a current knapsack for this run of the loop
    vector<snowglobe> currentKnapsack;
    int currentWeight = 0;
    int currentValue = 0;

    //put the things for this combination in the current knapsack
    for (int j = 0; j < houseSize; j++)
    {
      //cout << "j = " << j << endl;
      if (array[j])
      {
        currentKnapsack.push_back(house[j]);
        currentWeight += house[j].weight;
        currentValue += house[j].value;
      }
    }

    //see if this is valid, if so is it the best so far?
    if (currentWeight <= maxWeight && currentValue > bestValue)
    {
      bestKnapsack = currentKnapsack;
      bestValue = currentValue;
    }

    iterateArray(array, houseSize);
  }

  //clean up
  delete [] array;
  return bestKnapsack;
}

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
