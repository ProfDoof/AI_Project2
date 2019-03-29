#include <iostream>
#include "GACipher.h"

int main()
{
    int popSize = 100;

    // can either be 1 or 2. this is the choice between single-point or double-point crossover
    int crossPoints = 1; 

    // 0 -> no pruning (cut or flip)  | 1 -> cut  | 2 -> flip  (NOT MOD)
    int prune = 0;              
    int heuristic = 1;
    double mutationRate = .1;
    double copyRate = .1;
    double _time = 12 * 80000;
    double elitism = .1;
    std::string actualKey = "NVPJUEGRAQZCLKBWYDOHFISXTM";

    GACipher test(copyRate,
                mutationRate,
                _time,
                popSize,
                actualKey,
                crossPoints,
                prune,
                elitism,
                heuristic);


    double uniWeight = 1;
    double diWeight = 1.5;
    double triWeight = 3;

    test.setFreqWeights(uniWeight,diWeight,triWeight);
    test.run("../Test_Files/Cipher/test_1.txt");
    // test.setFreqWeights(uniWeight*4,diWeight*2,triWeight);
    // test.run("../Test_Files/Cipher/test_4.txt");
    // test.setFreqWeights(uniWeight*3,diWeight*2,triWeight);
    // test.run("../Test_Files/Cipher/test_5.txt");
}
