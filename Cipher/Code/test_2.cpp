#include <iostream>
//#include "GACipher.h"
#include "cipher_01.h"

using namespace std;

int main() 
{
	/*
	int generations;
	int popSize;
	int crossPoints;
	double mutationRate;
	bool elitism;
	
	
	cout << "Generations: ";
	cin >> generations;

	cout << "Population Size: ";
	cin >> popSize;

	cout << "Number of crossover points: ";
	cin >> crossPoints;
	
	cout << "Mutation Rate: ";
	cin >> mutationRate;

	cout << "Elitism (Y/N): ";
	cin >> elitism;
	*/


	// change prune to switching things around
	int generations = 10000;
	int popSize = 100;
	int crossPoints = 2;
	double mutationRate = 0.2;
	double copyRate = 0;
	bool elitism = 1;
	bool mutElitism = 1;
	string filename = "../Test_Files/test_1.txt";

	cout << "-------------------------------------------------" << endl << endl;

    GACipher test(generations, popSize, crossPoints, mutationRate, elitism);
	test.run(filename);

    cout << "DONE" << endl;
}