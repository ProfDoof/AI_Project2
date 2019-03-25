#include <iostream>
#include "GACipher.h"

int main()
{
    double copyRate = .1;
    double mutationRate = .1;
    int popSize = 100;
    double minutes = 60;
    minutes *= 60000;
    double uniWeight = 1;
    double diWeight = 1;
    double triWeight = 1;
    std::string actualKey = "NVPJUEGRAQZCLKBWYDOHFISXTM";
    GACipher test(copyRate,mutationRate,minutes,popSize,actualKey);
    test.setFreqWeights(uniWeight,diWeight,triWeight);
    test.run("../Test_Files/Cipher/test_1.txt");
}
