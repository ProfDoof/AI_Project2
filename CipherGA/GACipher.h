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
        std::map<std::string, double> freqChart;

        // Coded Message and Population
        std::string codedMessage;
        std::vector<std::pair<std::string,double>> population;

        // Random Engine
        std::mt19937 engine{static_cast<long unsigned int>(time(0))};
        std::uniform_real_distribution<double> dist{0.0, 1.0};

        // Alphabet
        std::vector<char> alphabet = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

    public:
        // Constructors
        GACipher();
        GACipher(double cR, double mR, double ttr, int pS);
        ~GACipher();

        // Getters and Setters
        double getCopyRate() { return copyRate; }
        void setCopyRate(double cR) { copyRate = cR; }

        double getMutationRate() { return mutationRate; }
        void setMutationRate(double mR) { mutationRate = mR; }

        double getPopSize() { return popSize; }
        void setPopSize(double pS) { popSize = pS; }

        // Methods
        // pre-run methods
        void loadCodedMessage(std::string fileName);
        void loadFreq();
        void randPopulation();

        // run methods
        double fitnessSet(std::string cipher);
        void catastrophicMutation(std::string lastString);
        bool mutate(std::pair<std::string, double>& ttm);

        // post-run methods

        // the actual method "run"
        void run(std::string filename);
};

// Constructors
GACipher::GACipher()
{

}

GACipher::GACipher(double cR, double mR, double ttr, int pS) : copyRate(cR), mutationRate(mR), timeToRun(ttr), popSize(pS)
{
    loadFreq();
    randPopulation();
}

// Destructor
GACipher::~GACipher()
{

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

void GACipher::loadCodedMessage(std::string fileName)
{

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

    for (int i = 0; i < tempMessage.size(); i++)
    {
        tempMessage[i] = alphabet[cipher.find(tempMessage[i])];
    }

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
    for (auto it = messFreqUni.begin(); it != messFreqUni.end(); it++)
    {
        if (freqChart.find(it->first)==freqChart.end())
        {
            freqChart[it->first] = 0;
        }

        double error = (it->second/tempMessage.size())-freqChart[it->first];
        error *= error;
        errorUni += error;
    }
    errorUni = sqrt(errorUni/messFreqUni.size());

    for (auto it = messFreqDi.begin(); it != messFreqDi.end(); it++)
    {
        if (freqChart.find(it->first)==freqChart.end())
        {
            freqChart[it->first] = 0;
        }

        double error = (it->second/tempMessage.size())-freqChart[it->first];
        error *= error;
        errorDi += error;
    }
    errorDi = sqrt(errorDi/messFreqDi.size());

    for (auto it = messFreqTri.begin(); it != messFreqTri.end(); it++)
    {
        if (freqChart.find(it->first)==freqChart.end())
        {
            freqChart[it->first] = 0;
        }

        double error = (it->second/tempMessage.size())-freqChart[it->first];
        error *= error;
        errorTri += error;
    }
    errorTri = sqrt(errorTri/messFreqTri.size());

    return (errorUni * .77)+(errorDi * .15)+(errorTri * .08);
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

// post-run methods

// the method "run"
void GACipher::run(std::string filename)
{
    loadCodedMessage(filename);

    // Declare the timer variables
    auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = startTime;
    std::chrono::duration<double, std::milli> compare = currentTime - startTime;
    std::pair<std::string, double> bestCipher("",0);

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
        if (population[0].second > bestCipher.second)
        {
            bestCipher = population[0];
        }

        // Build our intermediate population.
        for (int i = 0; intermediatePopulation.size() < popSize; i++, i %= popSize)
        {
            // ???????????? This needs work to work
            // correctly because the fitness is no
            // longer between 0 and 1 for certain.
            if (dist(engine) <= 1.05-(population[i].second/population.back().second)) {
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

                    // Now iterate through the partner
                    // string for both pushing each
                    // letter that isn't in the cross
                    // into the new cross.
                    for (int i = 0; i < chromoB.first.size(); i++)
                    {
                        bool flag = true;
                        for (int j = 0; flag && cross1.size(); j++)
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
                        for (int j = 0; flag && cross2.size(); j++)
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
    }
}
