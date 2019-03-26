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
#include <set>

class GACipher
{
    private:
        // Adjustable Knobs for the Genetic Algorithm
        int popSize;
        int crossovers;
        int prune;
        int elitism;
        double copyRate;
        double mutRate;
        double timeToRun;

        double uniWeight;
        double diWeight;
        double triWeight;

        std::map<std::string, double> freqChart;
        std::string actualKey;

        // Coded Message and Population
        std::string codedMessage;
        std::vector<std::pair<std::string,double>> population;
        std::pair<std::string, double> bestCipher;

        // Random Engine
        std::mt19937 engine{static_cast<long unsigned int>(time(0))};
        std::uniform_real_distribution<double> dist{0.0, 1.0};

        // Alphabet
        std::string alphabetString = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::set<char> alphabetSet = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
        std::vector<char> alphabet = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

    public:
        // CONSTRUCTORS
        GACipher(double _copyRate, double _mutRate, double _time, int _popSize, std::string _actualKey, int _crossovers, int _prune, double _elitism);

        void setFreqWeights(double uW, double dW, double tW);


        void singleCrossover(std::string &str, int crossPoint, std::string gen);
        void doubleCrossover(std::string &str, std::vector<int> crossPoints, std::string genA, std::string genB);

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
        std::string translate(std::string key);

        // post-run methods

        // the actual method "run"
        void run(std::string filename);
};

// CONSTRUCTOR
GACipher::GACipher(double _copyRate, double _mutRate, double _time, int _popSize, std::string _actualKey, int _crossovers, int _prune, double _elitism) {

    copyRate = _copyRate;
    mutRate = _mutRate;
    timeToRun = _time;
    popSize = _popSize;
    actualKey = _actualKey;
    crossovers = _crossovers;
    prune = _prune;
    elitism = popSize*_elitism;

    // DEFAULT WEIGHTS
    uniWeight = 1;
    diWeight = 1;
    triWeight = 1;
    bestCipher = std::make_pair("",0);
}


// "Complicated" Getter and/or Setter methods
void GACipher::setFreqWeights(double uW, double dW, double tW) {
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
    while (population.size() < popSize) {
        std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::pair<std::string, double> elem;

        // generate cipher key
        for (int i = 0; i < 26; i++) {
            int index = (dist(engine) * chars.size());
            elem.first += chars[index];
            chars.erase(index, 1);
        }
        elem.second = fitnessSet(elem.first);
        population.push_back(elem);
    }
}



// run methods
// INCOMPLETE (im sure there is more pruning to be done)
bool prune(std::string key, std::string translated) {
    std::vector<std::string> improbable_doubles = { "WW", "AA", "II", "BB", "UU", "ZZ", "KK", "XX", "VV", "JJ", "QQ" };

    for (int i = 1; i < translated.size(); i++)
        if (translated[i-1] == translated[i] && find(improbable_doubles.begin(), improbable_doubles.end(), translated.substr(i-1,2)) != improbable_doubles.end())
            return false;

    if (translated.back() == 'V' || translated.back() == 'J')
        return false;

    return true;
}


bool GACipher::mutate(std::pair<std::string, double>& ttm)
{
    bool mutated = false;
    for (int i = 0; i < 26; i++)
    {
        if (dist(engine) <= mutRate)
        {
            int randIndex = dist(engine) * 26;
            char temp = ttm.first[i];
            ttm.first[i] = ttm.first[randIndex];
            ttm.first[randIndex] = temp;
            mutated = true;
        }
    }
    return mutated;
}


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

std::string GACipher::translate(std::string key) {
    std::string output = "";

    for (int i = 0; i < codedMessage.size(); i++) {
        int index = alphabetString.find(codedMessage[i]);
        output += key[index];
    }
    return output;
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


double normalize(double n, double max, double min) {
    return (((0.95 - 0.05) * (n - min)) / (max - min)) + 0.05;
}

void GACipher::singleCrossover(std::string &str, int crossPoint, std::string gen) {
    std::string chars = str + gen.substr(crossPoint, 26 - crossPoint);
    std::set<char> charsIn;
    std::vector<char> charsOut;
    for_each(chars.begin(), chars.end(), [&charsIn](auto it) { if (charsIn.count(it) == 0) charsIn.insert(it); });
    set_difference(alphabetSet.begin(), alphabetSet.end(), charsIn.begin(), charsIn.end(), back_inserter(charsOut));

    for (int i = crossPoint; i < 26; i++) {
        if (str.find(gen[i]) == std::string::npos)
            str += gen[i];
        else {
            int index = dist(engine) * charsOut.size();
            str += charsOut[index];
            charsOut.erase(charsOut.begin() + index);
        }
    }
}

// we could easily make this multi
void GACipher::doubleCrossover(std::string &str, std::vector<int> crossPoints, std::string genA, std::string genB) {
    std::string chars =  str + genB.substr(crossPoints[0], crossPoints[1] - crossPoints[0]) + genA.substr(crossPoints[1], 25 - crossPoints[1]);
    std::set<char> charsIn;
    std::vector<char> charsOut;
    for_each(chars.begin(), chars.end(), [&charsIn](auto it) { if (charsIn.count(it) == 0) charsIn.insert(it); });
    set_difference(alphabetSet.begin(), alphabetSet.end(), charsIn.begin(), charsIn.end(), back_inserter(charsOut));

    for (int i = crossPoints[0]; i < crossPoints[1]; i++) {
        if (str.find(genB[i]) == std::string::npos)
            str += genB[i];
        else {
            int index = dist(engine) * charsOut.size();
            str += charsOut[index];
            charsOut.erase(charsOut.begin() + index);
        }
    }
    singleCrossover(str, crossPoints[1], genB);
}

// post-run methods

// the method "run"
void GACipher::run(std::string filename) {

    int elitismCopyAmount = popSize / 10;



    std::cout << "START RUN" << std::endl;
    loadCodedMessage(filename);
    loadFreq();
    randPopulation();
    sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second < b.second; });
    std::cout << "MESSAGE LOADED" << std::endl;


    // Declare the timer variables
    auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = startTime;
    std::chrono::duration<double, std::milli> compare = currentTime - startTime;

    // Run this as long as is defined in the GA
    // constructor
    int gen = 1;
    while (compare.count() < timeToRun) {
        std::vector<std::pair<std::string,double>> interPop;
        std::vector<std::pair<std::string,double>> nextPop;
        std::vector<std::pair<std::string,double>> normPop;

        // SORT POPULATION
        sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second < b.second; });

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

        for (int i = 0; i < elitism; i++)
        {
            nextPop.push_back(population[0]);
            population.erase(population.begin());
        }

        // BUILD INTERMEDIATE POPULATION
        for (int i = 0; interPop.size() < popSize-(elitism); i++, i %= popSize)
        {
            if (dist(engine) <= normalize(population[i].second, population.back().second, population[0].second)) {
                std::pair<std::string, double> temp(population[i]);
                interPop.push_back(temp);
            }
		}


        // Generate our new population.
        while (!interPop.empty())
        {
            // copyRate% chance of being pushed
            // directly into the next gen.
            if (dist(engine) < copyRate || interPop.size() == 1)
            {
                // Run the mutate function on the
                // currently being viewed population
                // pair
                // std::cout << intermediatePopulation[0].first << " " << intermediatePopulation[0].second << std::endl;
                bool flag = mutate(interPop[0]);
                // std::cout << intermediatePopulation[0].first << " " << intermediatePopulation[0].second << std::endl;

                // If the pair was mutated we need to
                // recalculate the fitness.
                if (flag)
                {
                    interPop[0].second = fitnessSet(interPop[0].first);
                }

                nextPop.push_back(interPop[0]);
                interPop.erase(interPop.begin());
            }
            // 100-copyRate% chance of crossover
            else
            {
                // crossPoints -> indices where crossover will occur
                // partnerIndex -> index of second genetor
                std::vector<int> crossPoints;
                crossPoints.push_back((dist(engine) * (24 / crossovers)) + 1);
                for (int i = 1; i < crossovers; i++)
                {
                    crossPoints.push_back((dist(engine) * (23 - (crossPoints[i-1]))) + crossPoints[i-1] + 1);
                }

                int partnerIndex = (dist(engine) * (interPop.size() - 1) + 1);



                // genetor A -> always at the first index charsIn the intermediate population
                // genetor B -> at a random index (not 0) charsIn the intermediate population
                std::string genitorA = interPop[0].first;
                std::string genitorB = interPop[partnerIndex].first;

                // std::cout << crossPoints[0] << " " << crossPoints[1] << " " << genitorA << " " << genitorB << std::endl;

                std::cout << "A: " << genitorA << " " << genitorA.size() << " " << crossPoints[0] << std::endl;
                std::cout << "B: " << genitorB << " " << genitorB.size() << " " << crossPoints[0] << std::endl;
                // first section of crossover
                std::string crossA = genitorA.substr(0, crossPoints[0]);
                std::string crossB = genitorB.substr(0, crossPoints[0]);

                if (crossovers == 1) {
                    singleCrossover(crossA, crossPoints[0], genitorB);
                    singleCrossover(crossB, crossPoints[0], genitorA);
                }
                else if (crossovers == 2) {
                    doubleCrossover(crossA, crossPoints, genitorA, genitorB);
                    doubleCrossover(crossB, crossPoints, genitorB, genitorA);
                }
                else {
                    std::cout << "The only available crossovers are single and double." << std::endl;
                    exit(EXIT_FAILURE);
                }



                //for (int i = 0; i < 26; i++) {
                //    if (count(crossA.begin(), crossA.end(), alphabet[i]) > 1 || count(crossB.begin(), crossB.end(), alphabet[i]) > 1) {
                //        cout << "FAILED" << endl;
                //        exit(EXIT_SUCCESS);
                //    }
                //}


                //out << crossPoints[0] << endl;
                //out << genitorA.substr(0, crossPoints[0]) << "  " << genitorA.substr(crossPoints[0], 26 - crossPoints[0])  << endl;
                //out << genitorB.substr(0, crossPoints[0]) << "  " << genitorB.substr(crossPoints[0], 26 - crossPoints[0])  << endl;
                //out << crossA.substr(0, crossPoints[0]) << "  " << crossA.substr(crossPoints[0], 26 - crossPoints[0])  << endl;
                //out << crossB.substr(0, crossPoints[0]) << "  " << crossB.substr(crossPoints[0], 26 - crossPoints[0])  << endl;

                //cout << crossPoints[0] << " " << crossPoints[1] << endl;
                //cout << genitorA.substr(0,crossPoints[0]) << " " << genitorA.substr(crossPoints[0], crossPoints[1] - crossPoints[0]) << " " << genitorA.substr(crossPoints[1], 26 - crossPoints[1]) << endl;
                //cout << genitorB.substr(0,crossPoints[0]) << " " << genitorB.substr(crossPoints[0], crossPoints[1] - crossPoints[0]) << " " << genitorB.substr(crossPoints[1], 26 - crossPoints[1]) << endl << endl;
                //cout << crossA.substr(0,crossPoints[0]) << " " << crossA.substr(crossPoints[0], crossPoints[1] - crossPoints[0]) << " " << crossA.substr(crossPoints[1], 26 - crossPoints[1]) << endl;
                //cout << crossB.substr(0,crossPoints[0]) << " " << crossB.substr(crossPoints[0], crossPoints[1] - crossPoints[0]) << " " << crossB.substr(crossPoints[1], 26 - crossPoints[1]) << endl;

                //exit(EXIT_SUCCESS);


                // remove genetors from interPop
                interPop.erase(interPop.begin() + partnerIndex);
                interPop.erase(interPop.begin());

                // create pairs so crossA/crossB can be added to population
                std::pair<std::string,double> elemA (crossA, fitnessSet(crossA));
                std::pair<std::string,double> elemB (crossB, fitnessSet(crossB));

                // mutate them
                mutate(elemA);
                mutate(elemB);

                // ad to new pop
                nextPop.push_back(elemA);
                nextPop.push_back(elemB);
            }

            population = nextPop;
        }

        // Get the current time
        currentTime = std::chrono::high_resolution_clock::now();
        compare = currentTime - startTime;
        gen++;
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
