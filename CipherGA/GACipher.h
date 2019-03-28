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

// construct uniFreq then access di and tri based on unireq

class GACipher
{
    private:
        // Adjustable Knobs for the Genetic Algorithm
        int popSize;
        int crossovers;
        int prune;
        int elitism;
        int heuristic;
        double copyRate;
        double mutRate;
        double timeToRun;

        double uniWeight;
        double diWeight;
        double triWeight;

        std::map<std::string, double> freqChart;
        std::map<std::string, double> uniFreq;
        std::map<std::string, double> diFreq;
        std::map<std::string, double> triFreq;
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
        std::vector<char> freqOrder = {'E','T','O','A','I','N','S','H','R','L','D','U','M','W','Y','C','F','G','B','P','V','K','X','J','Q','Z'};
        std::string mostFreqChars = "ETOAINSHRLDUMWYCFGBPVKXJQZ";

    public:
        // CONSTRUCTORS
        GACipher(double _copyRate, double _mutRate, double _time, int _popSize, std::string _actualKey, int _crossovers, int _prune, double _elitism, int _heuristic);

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
        double heuristic_1 (std::string cipher);
        double heuristic_2 (std::string cipher);
        double heuristic_3 (std::string cipher);
        void prunePop(std::vector<std::pair<std::string,double>> &population);

        void catastrophicMutation(std::string lastString);
        bool mutate(std::pair<std::string, double>& ttm);
        bool biasMutate(std::pair<std::string, double>& ttm);
        void printFitness();
        std::string translate(std::string key);

        // post-run methods

        // the actual method "run"
        void run(std::string filename);
};

// CONSTRUCTOR
GACipher::GACipher(double _copyRate, double _mutRate, double _time, int _popSize, std::string _actualKey, int _crossovers, int _prune, double _elitism, int _heuristic) {

    copyRate = _copyRate;
    mutRate = _mutRate;
    timeToRun = _time;
    popSize = _popSize;
    actualKey = _actualKey;
    crossovers = _crossovers;
    prune = _prune;
    elitism = popSize*_elitism;
    heuristic = _heuristic;

    // DEFAULT WEIGHTS
    uniWeight = 1;
    diWeight = 1;
    triWeight = 1;
    bestCipher = std::make_pair("",1000);
}

// "Complicated" Getter and/or Setter methods
void GACipher::setFreqWeights(double uW, double dW, double tW) {
    uniWeight = uW;
    diWeight = dW;
    triWeight = tW;
}

void readFile(std::string filename, std::map<std::string,double> &chart, int size) {
    std::ifstream fin;
    std::string line;

    fin.open(filename);

    if (fin.is_open())
    {
        int i = 0;
        while (getline(fin, line) && i < size)
        {
            std::pair<std::string, double> temp;
            std::stringstream ss(line);
            ss >> temp.first >> temp.second;
            chart.insert(temp);
            i++;
        }
    }
    fin.close();
}

// Pre-run methods
void GACipher::loadFreq()
{
    readFile("../Frequency/Frequency.txt", freqChart, 1000);
    readFile("../Frequency/Frequency_1.txt", uniFreq, 1000);
    readFile("../Frequency/Frequency_2.txt", diFreq, 50);
    readFile("../Frequency/Frequency_3.txt", triFreq, 50);
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
bool pruned(std::string key, std::string translated) {
    std::vector<std::string> improbable_doubles = { "WW", "AA", "II", "BB", "UU", "ZZ", "KK", "XX", "VV", "JJ", "QQ" };

    for (int i = 1; i < translated.size(); i++)
        if (translated[i-1] == translated[i] && find(improbable_doubles.begin(), improbable_doubles.end(), translated.substr(i-1,2)) != improbable_doubles.end())
            return true;

    if (translated.back() == 'V' || translated.back() == 'J')
        return true;

    for (int i = 1; i < translated.size(); i++)
        if (translated[i-1] == 'Q' && translated[i] != 'Q')
            return true;


    // could add QU

    return false;
}

void GACipher::prunePop(std::vector<std::pair<std::string,double>> &pop) {
    for (auto it = pop.begin(); it != pop.end(); it++) {
        if(pruned(it->first,translate(it->first))) {
            it->second += 2;
        }
    }
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


// FINTNESS 
double GACipher::heuristic_1 (std::string cipher) {
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
    // return errorUni + (errorDi * 3)+(errorTri * 9);*/
}

double GACipher::heuristic_2 (std::string cipher) {
    std::string translated = translate(cipher);

    // calculate uniError (|percievedFreq - observedFreq|)
    int occurances = 0;
    double frequency = 0;
    double uniError = 0;
    for (int i = 0; i < 26; i++) {

        // count the number of occurances a character in the translated text
        occurances = count(translated.begin(), translated.end(), cipher[i]);

        // find the frequency by dividing the number of occurances by the length of the the string
        double frequency = (1.0 * occurances) / translated.size();
        
        // uniError is expected frequency - observed frequency
        uniError += fabs(uniFreq[cipher.substr(i,1)] - frequency);
    }

    if (uniError < 0.8) {
        // calculate diError
        double diError = 0;
        std::set<std::string> alreadyCounted;
        for (int i = 1; i < translated.size(); i++) {
            std::string str = translated.substr(i - 1, 2);

            if (alreadyCounted.find(str) == alreadyCounted.end()) {
                // count the number of occurances a character in the translated text
                occurances = 0;

                for (int k = i; k < translated.size(); k++)
                    if (translated.substr(k - 1, 2) == str)
                        occurances++;

                // find the frequency by dividing the number of occurances by the length of the the string
                double frequency = (1.0 * occurances) / translated.size();
                diError += fabs(diFreq[str] - frequency);

                // add to set
                alreadyCounted.insert(str);
            }
        }

       // calculate triFit
        double triFit = 0;
        for (int i = 2; i < translated.size();i++) {
            triFit += triFreq[translated.substr(i - 2, 3)];
        } 

        return  ((uniError * uniWeight) + (diWeight * diError)) - (triWeight * triFit);
    }

    return 10 - uniError;
}

double GACipher::heuristic_3 (std::string cipher) {
    std::string translated = translate(cipher);
    
    // calculate uniError (|percievedFreq - observedFreq|)
    int occurances = 0;
    double frequency = 0;
    double uniError = 0;
    for (int i = 0; i < 26; i++) {

        // count the number of occurances a character in the translated text
        occurances = count(translated.begin(), translated.end(), cipher[i]);

        // find the frequency by dividing the number of occurances by the length of the the string
        double frequency = (1.0 * occurances) / translated.size();
        
        // uniError is expected frequency - observed frequency
        uniError += fabs(uniFreq[cipher.substr(i,1)] - frequency);
    }

    if (uniError < 0.8) {

        // calculate diError
        double diError = 0;
        std::set<std::string> alreadyCounted;
        for (int i = 1; i < translated.size(); i++) {
            std::string str = translated.substr(i - 1, 2);

            if (alreadyCounted.find(str) == alreadyCounted.end()) {
                // count the number of occurances a character in the translated text
                occurances = 0;

                for (int k = i; k < translated.size(); k++)
                    if (translated.substr(k - 1, 2) == str)
                        occurances++;

                // find the frequency by dividing the number of occurances by the length of the the string
                double frequency = (1.0 * occurances) / translated.size();
                diError += fabs(diFreq[str] - frequency);

                // add to set
                alreadyCounted.insert(str);
            }
        }

        // calculate diFit
        double diFit = 0;
        for (int i = 1; i < translated.size(); i++) {
            diFit += diFreq[translated.substr(i - 1, 2)];
        }
       
        // calculate triFit
        double triFit = 0;
        for (int i = 2; i < translated.size();i++) {
            triFit += triFreq[translated.substr(i - 2, 3)];
        }

        return uniError + diError - ((diFit * diWeight) + (triFit * triWeight));
    }
    return 10 - uniError;
}

double GACipher::fitnessSet(std::string cipher) {
    if (heuristic == 1)
        return heuristic_1(cipher);
    if (heuristic == 2)
        return heuristic_2(cipher);
    if (heuristic == 3)
        return heuristic_3(cipher);
}


std::string GACipher::translate(std::string key) {
    std::string output = "";

    for (int i = 0; i < codedMessage.size(); i++) {
        int index = alphabetString.find(codedMessage[i]);
        output += key[index];
    }
    return output;
}



void GACipher::printFitness() {
    for (int i = 0; i < population.size(); i++)
    {
        std::cout << population[i].second << std::endl;
    }
}

void GACipher::decode(std::string cipher) {
    std::string tempMessage = codedMessage;
    std::map<std::string, double> messFreqUni;
    std::map<std::string, double> messFreqDi;
    std::map<std::string, double> messFreqTri;

    // std::cout << "Cipher: " << cipher << std::endl;
    std::cout << "Coded Message:         " << tempMessage << std::endl;
    std::cout << "Coded Message Decoded: " << translate(cipher) << std::endl;
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

    std::cout << "START RUN" << std::endl;
    loadCodedMessage(filename);
    loadFreq();
    randPopulation();
    std::pair<std::string,double> elem;
    //elem.first = "EPVBLKXRTUCOJIZFASHMDNQWGY"; // test_1
    //elem.first = "NRJTAPYXLHGCWUODIKBVEQMFSZ"; // test_4
    //elem.first = "PKNCIDXLAQVWYOSGBFRZUJHMTE"; // test_5
    //elem.first = "RSPWQJBCOXIYMFVHLGKTZNAUED"; // test_6
    //elem.second = fitnessSet(elem.first);
    //population.push_back(elem);
    sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second < b.second; });
    //for_each(population.begin(), population.end(), [this](auto it) { std::cout << this->translate(it.first) << " " << it.second << std::endl; });
    //exit(EXIT_SUCCESS);
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

        if (prune && gen % 100 == 0 & gen > 20000) {
            prunePop(population);
        }

        // SORT POPULATION
        sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second < b.second; });
        //for_each(population.begin(), population.end(), [this](auto it) { std::cout << this->translate(it.first) << " " << it.second << std::endl; }); std::cout << std::endl << std::endl;// cout << i << endl;
        //exit(EXIT_SUCCESS);


        // FIND BEST (lower is better)
        if (population[0].second < bestCipher.second)
        {
            std::set<std::string> unique;
            for_each(population.begin(), population.end(), [&unique](auto it) { unique.insert(it.first); });
            std::cout << "Gen: " << gen << std::endl;
            std::cout << "New Best: " << population[0].first << std::endl << "Fitness: " << population[0].second << std::endl;
            std::cout << "Unique Values in Population: " << unique.size() << std::endl;
           
            bestCipher = population[0];
            decode(bestCipher.first);
            std::cout << std::endl;
        }

        // ELITISM
        for (int i = 0; i < elitism; i++)
        {
            nextPop.push_back(population[i]);
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
                bool flag = mutate(interPop[0]);

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

                // first section of crossover
                std::string crossA = genitorA.substr(0, crossPoints[0]);
                std::string crossB = genitorB.substr(0, crossPoints[0]);

                // secod section of crossover
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

                // remove genetors from interPop
                interPop.erase(interPop.begin() + partnerIndex);
                interPop.erase(interPop.begin());

          
                // create pairs so crossA/crossB can be added to population
                std::pair<std::string,double> elemA (crossA, 0);
                std::pair<std::string,double> elemB (crossB, 0);

                mutate(elemA);
                mutate(elemB);

                elemA.second = fitnessSet(elemA.first);
                elemB.second = fitnessSet(elemB.first);
           
                // add to new pop
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
