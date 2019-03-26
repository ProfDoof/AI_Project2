#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <chrono>
#include <algorithm>

class GACipher
{
    private:
        // Adjustable Knobs for the Genetic Algorithm
        double copyRate;
        double mutationRate;
        double timeToRun;
        int popSize;
        double uniWeight;
        double diWeight;
        double triWeight;
        std::map<std::string, double> freqChart;
        std::string actualKey;

        // Coded Message and Population
        std::string codedMessage;
        std::vector<std::pair<std::string,double>> population;
        std::pair<std::string, double> bestCipher("",0);

        // Random Engine
        std::mt19937 engine{static_cast<long unsigned int>(time(0))};
        std::uniform_real_distribution<double> dist{0.0, 1.0};

        // Alphabet
        std::vector<char> alphabet = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

    public:
        // Constructors
        GACipher();
        GACipher(double cR, double mR, double ttr, int pS,std::string aK);
        ~GACipher();

        // Getters and Setters
        double getCopyRate() { return copyRate; }
        void setCopyRate(double cR) { copyRate = cR; }

        double getMutationRate() { return mutationRate; }
        void setMutationRate(double mR) { mutationRate = mR; }

        double getPopSize() { return popSize; }
        void setPopSize(double pS) { popSize = pS; }

        void setFreqWeights(double uW, double dW, double tW);

        // Methods
        // pre-run methods
        void loadCodedMessage(std::string filename);
        void loadFreq();
        void randPopulation();
        void decode(std::string cipher);

        // run methods
        double fitnessSet(std::string cipher);
        void catastrophicMutation(std::string lastString);
        bool mutate(std::pair<std::string, double>& ttm);
        void printFitness();

        // post-run methods

        // the actual method "run"
        void run(std::string filename);
};

// Constructors
GACipher::GACipher()
{

}

GACipher::GACipher(double cR, double mR, double ttr, int pS, std::string aK) : copyRate(cR), mutationRate(mR), timeToRun(ttr), popSize(pS), actualKey(aK)
{
    uniWeight = 1;
    diWeight = 1;
    triWeight = 1;
}

// Destructor
GACipher::~GACipher()
{

}

// "Complicated" Getter and/or Setter methods
void GACipher::setFreqWeights(double uW, double dW, double tW)
{
    uniWeight = uW;
    diWeight = dW;
    triWeight = tW;
}

// Pre-run methods
void GACipher::loadFreq()
{
    std::ifstream fin;
    std::string line;

    fin.open("../Frequency/Frequency.txt");

    if (fin.is_open())
    {
        while (getline(fin, line))
        {
            std::pair<std::string, double> temp;
            std::stringstream ss(line);
            ss >> temp.first >> temp.second;
            freqChart.insert(temp);
        }
    }

    fin.close();
}

void GACipher::loadCodedMessage(std::string filename)
{
    std::ifstream fin;
    fin.open(filename);
    getline(fin, codedMessage);
    for (int i = 0; i < codedMessage.size(); i++ )
    {
        codedMessage[i] = toupper(codedMessage[i]);
    }
    //std::cout << codedMessage << std::endl;
    fin.close();
}

void GACipher::randPopulation()
{
    std::vector<char> genGeneList;
    int randIndex;
    std::pair<std::string, double> temp;
    while (population.size() <= popSize)
    {
        genGeneList = alphabet;
        temp.first = "";
        for (int i = 0; i < 26; i++)
        {
            randIndex = (dist(engine)*genGeneList.size());
            temp.first += genGeneList[randIndex];
            genGeneList.erase(genGeneList.begin()+randIndex);
        }
        // std::cout << temp.first << std::endl;
        temp.second = fitnessSet(temp.first);
        population.push_back(temp);
    }
}

// run methods
double GACipher::fitnessSet(std::string cipher)
{
    std::string tempMessage = codedMessage;
    std::map<std::string, double> messFreqUni;
    std::map<std::string, double> messFreqDi;
    std::map<std::string, double> messFreqTri;

    // std::cout << "Cipher: " << cipher << std::endl;
    // std::cout << "TempMessageB: " << tempMessage << std::endl;
    for (int i = 0; i < tempMessage.size(); i++)
    {
        tempMessage[i] = alphabet[cipher.find(tempMessage[i])];
    }
    // std::cout << "TempMessageA: " << tempMessage << std::endl;

    // Add all unigraphs, digraphs, and trigraphs to
    // their own frequency chart.
    for (int i = 0; i < tempMessage.size(); i++)
    {
        if (messFreqUni.find(tempMessage.substr(i,1)) == messFreqUni.end()){
            messFreqUni.insert(std::pair<std::string, double>(tempMessage.substr(i,1), 1));
        }
        else {
            messFreqUni[tempMessage.substr(i,1)] += 1;
        }

        if (i > 0)
        {
            if (messFreqDi.find(tempMessage.substr(i-1,2)) == messFreqDi.end()){
                messFreqDi.insert(std::pair<std::string, double>(tempMessage.substr(i-1,2), 1));
            }
            else {
                messFreqDi[tempMessage.substr(i-1,2)] += 1;
            }
        }

        if (i > 1)
        {
            if (messFreqTri.find(tempMessage.substr(i-2,3)) == messFreqTri.end()){
                messFreqTri.insert(std::pair<std::string, double>(tempMessage.substr(i-2,3), 1));
            }
            else {
                messFreqTri[tempMessage.substr(i-2,3)] += 1;
            }
        }
    }

    // Calculate the Sum of Squares error for
    // each type of frequency chart.
    double errorUni = 0, errorDi = 0, errorTri = 0;
    for (auto it = freqChart.begin(); it != freqChart.end(); it++)
    {
        double error = 0;
        if (it->first.size() == 1)
        {
            if (messFreqUni.find(it->first)==messFreqUni.end())
            {
                messFreqUni[it->first] = 0;
            }

            error = (it->second)-(messFreqUni[it->first]/(tempMessage.size()-1));
            error *= error;
            errorTri += error;
        }
        else if (it->first.size() == 2)
        {
            if (messFreqDi.find(it->first)==messFreqDi.end())
            {
                messFreqDi[it->first] = 0;
            }

            error = (it->second)-(messFreqDi[it->first]/(tempMessage.size()-1));
            error *= error;
            errorTri += error;
        }
        else
        {
            if (messFreqTri.find(it->first)==messFreqTri.end())
            {
                messFreqTri[it->first] = 0;
            }

            error = (it->second)-(messFreqTri[it->first]/(tempMessage.size()-1));
            error *= error;
            errorTri += error;
        }
    }
    errorUni = sqrt(errorUni/26);
    errorDi = sqrt(errorDi/57);
    errorTri = sqrt(errorTri/115);

    return (errorUni*uniWeight) + (errorDi*diWeight) + (errorTri*triWeight);
    // Sum of squares error looking from the perspective of the frequency chart generated for the code. Let's try the other direction why don't we.
    // double errorUni = 0, errorDi = 0, errorTri = 0;
    // for (auto it = messFreqUni.begin(); it != messFreqUni.end(); it++)
    // {
    //     double error = 0;
        // if (freqChart.find(it->first)!=freqChart.end())
        // {
        //     error = (it->second/tempMessage.size())-freqChart[it->first];
        // }
    //
    //
    //     error *= error;
    //     errorUni += error;
    // }
    // errorUni = sqrt(errorUni/messFreqUni.size());
    //
    // for (auto it = messFreqDi.begin(); it != messFreqDi.end(); it++)
    // {
    //     double error = 0;
    //     if (freqChart.find(it->first)!=freqChart.end())
    //     {
    //         error = (it->second/(tempMessage.size()-1))-freqChart[it->first];
    //     }
    //
    //     error *= error;
    //     errorDi += error;
    // }
    // errorDi = sqrt(errorDi/messFreqDi.size());
    //
    // for (auto it = messFreqTri.begin(); it != messFreqTri.end(); it++)
    // {
    //     double error = 0;
    //     if (freqChart.find(it->first)!=freqChart.end())
    //     {
    //         error = (it->second/(tempMessage.size()-2))-freqChart[it->first];
    //     }
    //
    //     error *= error;
    //     errorTri += error;
    // }
    // errorTri = sqrt(errorTri/messFreqTri.size());
    //
    // return errorUni + (errorDi * 3)+(errorTri * 9);
}

bool GACipher::mutate(std::pair<std::string, double>& ttm)
{
    bool ttr = false;
    for (int i = 0; i < ttm.first.size(); i++)
    {
        if (dist(engine) <= mutationRate)
        {
            int randIndex = dist(engine)*ttm.first.size();
            char temp = ttm.first[i];
            ttm.first[i] = ttm.first[randIndex];
            ttm.first[randIndex] = temp;
            ttr = true;
        }
    }

    return ttr;
}

void GACipher::printFitness()
{
    for (int i = 0; i < population.size(); i++)
    {
        std::cout << population[i].second << std::endl;
    }
}

void GACipher::decode(std::string cipher)
{
    std::string tempMessage = codedMessage;
    std::map<std::string, double> messFreqUni;
    std::map<std::string, double> messFreqDi;
    std::map<std::string, double> messFreqTri;

    // std::cout << "Cipher: " << cipher << std::endl;
    std::cout << "Coded Message:         " << tempMessage << std::endl;
    for (int i = 0; i < tempMessage.size(); i++)
    {
        tempMessage[i] = alphabet[cipher.find(tempMessage[i])];
    }
    std::cout << "Coded Message Decoded: " << tempMessage << std::endl;
}

// post-run methods

// the method "run"
void GACipher::run(std::string filename)
{
    std::cout << "Start Run" << std::endl;
    loadCodedMessage(filename);
    loadFreq();
    randPopulation();
    std::cout << "Message Loaded" << std::endl << std::endl;
    // printFitness();

    // Declare the timer variables
    auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = startTime;
    std::chrono::duration<double, std::milli> compare = currentTime - startTime;

    // Run this as long as is defined in the GA
    // constructor
    while (compare.count() < timeToRun)
    {
        // declare our intermediate population
        // and our new population.
        std::vector<std::pair<std::string,double>> intermediatePopulation;
        std::vector<std::pair<std::string,double>> newPopulation;

        // Sort our population by fitness.
        sort(population.begin(), population.end(),
            [](const std::pair<std::string, double> a,
               const std::pair<std::string, double> b)
            {
                return a.second < b.second;
            });


        // Check and see if we have found a better
        // fitting cipher than our current best
        // in this case lower is better (it's like
        // golf)
        if (bestCipher.first == "" || population[0].second < bestCipher.second)
        {
            bestCipher = population[0];
            std::cout << "New Best: " << bestCipher.first << std::endl << "Fitnes: " << bestCipher.second << std::endl;
            decode(bestCipher.first);
            std::cout << std::endl;
        }

        // Build our intermediate population.
        for (int i = 0; intermediatePopulation.size() < popSize; i++, i %= popSize)
        {
            // ???????????? This needs work to work
            // correctly because the fitness is no
            // longer between 0 and 1 for certain.
            if (dist(engine) <= 1.00-population[i].second) {
                std::pair<std::string, double> temp(population[i]);
                intermediatePopulation.push_back(temp);
            }
		}

        // Generate our new population.
        while (!intermediatePopulation.empty())
        {
            // copyRate% chance of being pushed
            // directly into the next gen.
            if (dist(engine) < copyRate || intermediatePopulation.size() == 1)
            {
                // Run the mutate function on the
                // currently being viewed population
                // pair
                // std::cout << intermediatePopulation[0].first << " " << intermediatePopulation[0].second << std::endl;
                bool flag = mutate(intermediatePopulation[0]);
                // std::cout << intermediatePopulation[0].first << " " << intermediatePopulation[0].second << std::endl;

                // If the pair was mutated we need to
                // recalculate the fitness.
                if (flag)
                {
                    intermediatePopulation[0].second = fitnessSet(intermediatePopulation[0].first);
                }

                newPopulation.push_back(intermediatePopulation[0]);
                intermediatePopulation.erase(intermediatePopulation.begin());
            }
            // 100-copyRate% chance of crossover
            else
            {
                int crossOverIndex = dist(engine)*26;
                int partnerIndex = dist(engine)*intermediatePopulation.size();
                std::string cross1;
                std::string cross2;

                // If the partnerIndex is not equal to
                // zero then we can crossover. If it is
                // equal to zero we can not crossover
                // so let's just push it directly through
                if (partnerIndex != 0)
                {
                    // Collect the two pairs to be
                    // crossed.
                    std::pair<std::string, double> chromoA = intermediatePopulation[0];
                    std::pair<std::string, double> chromoB = intermediatePopulation[partnerIndex];

                    // Put in the first part of each
                    // set.
                    cross1 = chromoA.first.substr(0,crossOverIndex);
                    cross2 = chromoB.first.substr(0,crossOverIndex);
                    // std::cout << cross1 << std::endl;
                    // std::cout << cross2 << std::endl;

                    // Now iterate through the partner
                    // string for both pushing each
                    // letter that isn't in the cross
                    // into the new cross.
                    for (int i = 0; i < chromoB.first.size(); i++)
                    {
                        bool flag = true;
                        for (int j = 0; flag && j < cross1.size(); j++)
                        {
                            if (cross1[j] == chromoB.first[i])
                            {
                                flag = false;
                            }
                        }

                        if (flag)
                        {
                            cross1 += chromoB.first[i];
                        }
                    }

                    for (int i = 0; i < chromoA.first.size(); i++)
                    {
                        bool flag = true;
                        for (int j = 0; flag && j < cross2.size(); j++)
                        {
                            if (cross2[j] == chromoA.first[i])
                            {
                                flag = false;
                            }
                        }

                        if (flag)
                        {
                            cross2 += chromoA.first[i];
                        }
                    }
                    // std::cout << cross1 << std::endl;
                    // std::cout << cross2 << std::endl;

                    intermediatePopulation.erase(intermediatePopulation.begin()+partnerIndex);
                    intermediatePopulation.erase(intermediatePopulation.begin());

                    chromoA.first = cross1;
                    chromoB.first = cross2;
                    mutate(chromoA);
                    mutate(chromoB);

                    newPopulation.push_back(chromoA);
                    newPopulation.push_back(chromoB);
                }
                else
                {
                    // Run the mutate function on the
                    // currently being viewed population
                    // pair
                    bool flag = mutate(intermediatePopulation[0]);

                    // If the pair was mutated we need to
                    // recalculate the fitness.
                    if (flag)
                    {
                        intermediatePopulation[0].second = fitnessSet(intermediatePopulation[0].first);
                    }

                    newPopulation.push_back(intermediatePopulation[0]);
                    intermediatePopulation.erase(intermediatePopulation.begin());
                }
            }

            population = newPopulation;
        }

        // Get the current time
        currentTime = std::chrono::high_resolution_clock::now();
        compare = currentTime - startTime;
        break;
    }

    std::cout << "\nThe best key found based on fitness was " << bestCipher.first << std::endl;
    std::cout << "The actual key should be                " << actualKey;
    std::cout << "\nThe fitness for the actual key was      " << fitnessSet(actualKey);
    std::cout << "\nThe number of same letters between the best and the actual key is ";
    int count = 0;
    for (int i = 0; i < 26; i++)
    {
        if (bestCipher.first[i]==actualKey[i])
        {
            count++;
        }
    }
    std::cout << count << std::endl << std::endl;
}
