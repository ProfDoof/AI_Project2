#include <iostream>
#include "GACipher.h"

int main()
{
    double copyRate = .5;
    double mutationRate = .05;
    int popSize = 500;
    double minutes = 2;
    minutes *= 60000;
    double uniWeight = 1;
    double diWeight = 1;
    double triWeight = 1;
    std::string actualKey = "NVPJUEGRAQZCLKBWYDOHFISXTM";
    GACipher test(copyRate,mutationRate,minutes,popSize,actualKey);
    test.setFreqWeights(uniWeight,diWeight,triWeight);
    test.run("../Test_Files/Cipher/test_1.txt");
    test.setFreqWeights(uniWeight*4,diWeight*2,triWeight);
    test.run("../Test_Files/Cipher/test_1.txt");
    test.setFreqWeights(uniWeight*3,diWeight*2,triWeight);
    test.run("../Test_Files/Cipher/test_1.txt");
}
