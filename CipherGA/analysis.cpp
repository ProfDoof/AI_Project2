#include "GACipher.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/wait.h>

using namespace std;

int main()
{
    int popSize = 100;


    // can either be 1 or 2. this is the choice between single-point or double-point crossover
    int crossPoints[] = {1,2};

    // 0 -> no pruning (cut or flip)  | 1 -> cut  | 2 -> flip  (NOT MOD)
    int prune = 1;
    int heuristic[] = {1,2,3};
    double mutationRate[] = {0,.1,.25};
    double copyRate[] = {0,.1};
    double _time = .01 * 60000;
    double elitism[] = {0,.1};
    std::string filename = "../Test_Files/Cipher/test_1.txt";
    std::string actualKey = "NVPJUEGRAQZCLKBWYDOHFISXTM";
    double uniWeight = 1;
    double diWeight = 1.5;
    double triWeight = 3;
    GACipher analysisSet[72];
    int count = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 2; l++)
                {
                    for (int m = 0; m < 2; m++)
                    {
                        GACipher temp(copyRate[l],
                                    mutationRate[k],
                                    _time,
                                    popSize,
                                    actualKey,
                                    crossPoints[i],
                                    prune,
                                    elitism[m],
                                    heuristic[j]);
                        analysisSet[count] = temp;
                        count++;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 72; i++)
    {
        pid_t pid = fork();
        if (pid > 0)
        {
            analysisSet[i].loadVersion(i);
            analysisSet[i].run(filename);
            i = 72;
        }
        else if (pid == 0)
        {
            cout << "fork succeeded for " << i << endl;
        }
        else
        {
            cout << "fork failed for " << i << endl;
        }
    }

    while (true)
    {
        int status;
        pid_t done = wait(&status);
        if (done == -1)
        {
            if (errno == ECHILD) break; // no more child processes
        }
        else
        {
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            {
                cerr << "pid " << done << " failed" << endl;
                exit(1);
            }
        }
    }
}
