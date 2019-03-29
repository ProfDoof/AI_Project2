#include <iostream>
#include "GACipher.h"

int main()
{
    int popSize = 100;

    // can either be 1 or 2. this is the choice between single-point or double-point crossover
    int crossPoints = 1;

    // 0 -> no pruning (cut or flip)  | 1 -> cut  | 2 -> flip  (NOT MOD)
    int prune = 0;
    int heuristic = 3;
    double mutationRate = .1;
    double copyRate = .1;
    double _time = 12 * 60000;
    double elitism = .1;
    std::string filename = "../Test_Files/Cipher/test_1.txt";
    std::string actualKey = "NVPJUEGRAQZCLKBWYDOHFISXTM";
    double uniWeight = 1;
    double diWeight = 1.5;
    double triWeight = 3;

    std::string yn;
    std::cout << "Would you like to set the variables yourself? (Y/N) ";
    std::cin >> yn;
    if (yn == "Y")
    {
        std::cout << "What population size would you like? ";
        std::cin >> popSize;
        std::cout << "How many crossover points would you like? (1/2) ";
        std::cin >> crossPoints;
        std::cout << "Would you like to prune? (0/1)";
        std::cin >> prune;
        std::cout << "Which heuristic would you like to use? (1/2/3) ";
        std::cin >> heuristic;
        std::cout << "What mutation rate would you like? (Value must be between 0 and 1 inclusive) ";
        std::cin >> mutationRate;
        std::cout << "What copy rate would you like? (Value must be between 0 and 1 inclusive) ";
        std::cin >> copyRate;
        std::cout << "How many minutes would like to run it for? ";
        std::cin >> _time;
        _time *= 60000;
        std::cout << "What percentage of the elite would you like to pass into the next generation? (Value must be between 0 and 1 inclusive) ";
        std::cin >> elitism;
        std::cout << "What coded message would you like to attempt to decode? (relative path from this executable to the test files)" << std::endl;
        std::cout << "Enter filename: ";
        std::cin >> filename;
        std::cout << "What is the actual key without any spaces for the coded message you are attempting to decode? (can be found in the meta file for the test you are running) " << std::endl << "Enter actual key: ";
        std::cin >> actualKey;

    }

    GACipher test(copyRate,
                mutationRate,
                _time,
                popSize,
                actualKey,
                crossPoints,
                prune,
                elitism,
                heuristic);



    test.setFreqWeights(uniWeight,diWeight,triWeight);
    test.run(filename);
    // test.setFreqWeights(uniWeight*4,diWeight*2,triWeight);
    // test.run("../Test_Files/Cipher/test_4.txt");
    // test.setFreqWeights(uniWeight*3,diWeight*2,triWeight);
    // test.run("../Test_Files/Cipher/test_5.txt");

}
