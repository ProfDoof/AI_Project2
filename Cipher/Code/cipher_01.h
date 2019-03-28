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
#include <random>
#include <time.h>
#include <set>
#include <iterator>
#include <iomanip>

using namespace std;

class GACipher
{
    private:
        // ADJUSTABLE KNOBS
        int generations;
        int popSize;
        int crossovers;
        double mutRate;
        bool elitism;

        // ENCODED MESSAGE
        string encodedString;
        std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


        
       
       
        //std::map<std::string, double> freqChart;
        //std::map<char, double> uniFreqMapAvg;
        std::map<char, double> uniFreqMap;
        std::map<std::string, double> diFreqMap;
        std::map<std::string, double> triFreqMap;

        vector<pair<char,double>> encodedFreq;


        std::vector<std::pair<std::string,double>> uniFreq;
        std::vector<std::pair<std::string,double>> diFreq;
        std::vector<std::pair<std::string,double>> triFreq;


        // Coded Message and Population
       
        std::vector<std::pair<std::string,double>> population;

        // Random Engine
        std::mt19937 engine{static_cast<long unsigned int>(time(0))};
        std::uniform_real_distribution<double> dist{0.0, 1.0};

        // Alphabet
        std::set<char> alphabetSet = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

    public:
        // CONSTRUCTOR
        GACipher(int generations, int popSize, int crossovers, double mutRate, bool elitism);

        // METHODS
        void singleCrossover(string &str, int crossPoint, string gen);
        void doubleCrossover(string &str, vector<int> crossPoints, string genA, string genB);





        // Methods
        // pre-run methods
        void loadCodedMessage(std::string filename);
        void loadFreq();
        void randPopulation();
        void decode(std::string cipher);
        std::string translate(std::string key);

        // run methods
        double fitnessSet(std::string cipher);
        void catastrophicMutation(std::string lastString);
        bool mutate(std::pair<std::string, double>& ttm);
        void printFitness();

        // post-run methods

        // the actual method "run"
        void run(std::string filename);
};

// Constructor
GACipher::GACipher(int _generations, int _popSize, int _crossovers, double _mutRate, bool _elitism) {
	generations = _generations;
    popSize = _popSize;
    crossovers = _crossovers;
    mutRate = _mutRate;
    elitism = _elitism;
}

// COMPLETE
void GACipher::loadFreq() {
    std::ifstream fin;
    std::string line;

    fin.open("../Frequency/Frequency_1.txt");
    if (fin.is_open()) {
        while (getline(fin, line))
        {
            std::pair<string, double> temp;
            std::pair<char, double> temp_1;

            std::stringstream ss(line);
            ss >> temp.first >> temp.second;
            
            temp_1.first = *(temp.first.c_str());
            temp_1.second = temp.second;

            uniFreq.push_back(temp);
            uniFreqMap.insert(temp_1);
        }
    }
    fin.close();

    int i = 0;
    fin.open("../Frequency/Frequency_2.txt");
    if (fin.is_open()) {
        while (getline(fin, line) && i < 40)
        {
            std::pair<string, double> temp;
            std::stringstream ss(line);

            ss >> temp.first >> temp.second;
            diFreqMap.insert(temp);

            i++;
        }
    }
    fin.close();

    i = 0;
    fin.open("../Frequency/Frequency_3.txt");
    if (fin.is_open()) {
        while (getline(fin, line) && i < 15)
        {
            std::pair<string, double> temp;
            std::stringstream ss(line);

            ss >> temp.first >> temp.second;
            triFreqMap.insert(temp);
            i++;
        }
    }
    fin.close();
}

// COMLPLETE
void GACipher::loadCodedMessage(std::string filename) {
    std::ifstream fin;
    fin.open(filename);
    getline(fin, encodedString);
    fin.close();
}

// COMPLETE
void GACipher::randPopulation() {
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

// COMPLETE
std::string GACipher::translate(std::string key) {
    string output = "";

    for (int i = 0; i < encodedString.size(); i++) {
        int index = alphabet.find(encodedString[i]);
        output += key[index];
    }
    return output;
}

// INCOMPLETE (im sure there is more pruning to be done)
bool prune(string key, string translated) {
    vector<string> improbable_doubles = { "WW", "AA", "II", "BB", "UU", "ZZ", "KK", "XX", "VV", "JJ", "QQ" };
    
    for (int i = 1; i < translated.size(); i++)
        if (translated[i-1] == translated[i] && find(improbable_doubles.begin(), improbable_doubles.end(), translated.substr(i-1,2)) != improbable_doubles.end())
            return false;

    if (translated.back() == 'V' || translated.back() == 'J')
        return false;

    return true;
}

// INCOMPLETE (this might need to return nothing)
bool GACipher::mutate(std::pair<std::string, double>& elem) {
    bool mutated = false;
    for (int i = 0; i < 26; i++) {
        if (dist(engine) <= mutRate) {
            int randIndex = dist(engine) * 26;
            char temp = elem.first[i];
            elem.first[i] = elem.first[randIndex];
            elem.first[randIndex] = temp;
            mutated = true;
            break;
        }
    }
    return mutated;
}

double calcFit() {
    int occurances = 0;
    double frequency = 0;
    double error = 0;
}


// run methods
double GACipher::fitnessSet(std::string key)
{   
       //key = "PKNCIDXLAQVWYOSGBFRZUJHMTE"; // test 5
    //key = "RSPWQJBCOXIYMFVHLGKTZNAUED"; // test 6
    string translated = translate(key);

    // calculate uniError (|percievedFreq - observedFreq|)
    int occurances = 0;
    double frequency = 0;
    double uniError = 0;
    for (int i = 0; i < 26; i++) {

        // count the number of occurances a character in the translated text
        occurances = count(translated.begin(), translated.end(), key[i]);

        // find the frequency by dividing the number of occurances by the length of the the string
        double frequency = (1.0 * occurances) / translated.size();
        
        // uniError is expected frequency - observed frequency
        uniError += abs(uniFreqMap[key[i]] - frequency);

        //cout << setprecision(4);
        //cout << key[i] << " " << setw(10) << frequency << "\t" << key[i] << " " << setw(10) << uniFreqMap[key[i]] << "\t" << setw(10) << abs(uniFreqMap[key[i]] - frequency) << "\t" << setw(10) << uniError << endl;
    }

    // calculate diError
    double diError = 0;
    set<string> alreadyCounted;
    for (int i = 1; i < translated.size(); i++) {
        string str = translated.substr(i - 1, 2);

        if (alreadyCounted.find(str) == alreadyCounted.end()) {
            // count the number of occurances a character in the translated text
            occurances = 0;

            for (int k = i; k < translated.size(); k++)
                if (translated.substr(k - 1, 2) == str)
                    occurances++;

            // find the frequency by dividing the number of occurances by the length of the the string
            double frequency = (1.0 * occurances) / translated.size();
            
            // uniError is expected frequency - observed frequency
            diError += abs(diFreqMap[str] - frequency);

            // add to set
            alreadyCounted.insert(str);

            //cout << setprecision(4);
            //cout << str << " " << setw(10) << frequency << "\t" << str << " " << setw(10) << diFreqMap[str] << "\t" << setw(10) << abs(diFreqMap[str] - frequency) << "\t" << setw(10) << diError << endl;
        }
    }

    
    // calculate triError
    double triFit = 0;
    for (auto it = triFreqMap.begin(); it != triFreqMap.end(); it++) {

        // count the number of occurances a character in the translated text
        occurances = 0;

        for (int k = 2; k < translated.size(); k++)
            if (translated.substr(k - 2, 3) == it->first)
                occurances++;
        
        // uniError is expected frequency - observed frequency
        triFit += occurances * it->second;
    }

    int factor = 1;
    //if (!prune(key, translated))
    //    factor *= 5;

    //cout << uniError << " " << diError << " " << 3 * triFit << endl;
    //cout << 5 - (factor * (uniError + (1.5 * diError))) + triFit << endl;
    //exit(EXIT_SUCCESS);

    return 5 - (factor * (uniError + (1.5 * diError))) + (3.5 * triFit);
}



double normalize(double n, double max, double min) {
    return (((0.95 - 0.05) * (n - min)) / (max - min)) + 0.05;
}

void GACipher::singleCrossover(string &str, int crossPoint, string gen) {
    string chars = str + gen.substr(crossPoint, 26 - crossPoint);
    set<char> charsIn;
    vector<char> charsOut;
    for_each(chars.begin(), chars.end(), [&charsIn](auto it) { if (charsIn.count(it) == 0) charsIn.insert(it); });
    set_difference(alphabetSet.begin(), alphabetSet.end(), charsIn.begin(), charsIn.end(), back_inserter(charsOut));

    for (int i = crossPoint; i < 26; i++) {
        if (str.find(gen[i]) == string::npos)
            str += gen[i];
        else {
            int index = dist(engine) * charsOut.size();
            str += charsOut[index];
            charsOut.erase(charsOut.begin() + index);
        }
    }
}

// we could easily make this multi
void GACipher::doubleCrossover(string &str, vector<int> crossPoints, string genA, string genB) {
    string chars =  str + genB.substr(crossPoints[0], crossPoints[1] - crossPoints[0]) + genA.substr(crossPoints[1], 25 - crossPoints[1]);
    set<char> charsIn;
    vector<char> charsOut;
    for_each(chars.begin(), chars.end(), [&charsIn](auto it) { if (charsIn.count(it) == 0) charsIn.insert(it); });
    set_difference(alphabetSet.begin(), alphabetSet.end(), charsIn.begin(), charsIn.end(), back_inserter(charsOut));

    for (int i = crossPoints[0]; i < crossPoints[1]; i++) {
        if (str.find(genB[i]) == string::npos)
            str += genB[i];
        else {
            int index = dist(engine) * charsOut.size();
            str += charsOut[index];
            charsOut.erase(charsOut.begin() + index);
        }
    }
    singleCrossover(str, crossPoints[1], genB);
}


// the method "run"
void GACipher::run(std::string filename)
{
    // ADJUST KNOBS
    int elitismCopyAmount = popSize / 10;



    std::cout << "START RUN" << std::endl;
    

    loadCodedMessage(filename);
    loadFreq();
    //for_each(uniFreq.begin(), uniFreq.end(), [this](auto it) { cout << this->translate(it.first) << " " << it.second << endl; });

    //pair<string,double> elem;
    //elem.first = "EPVBLKXRTUCOJIZFASHMDNQWGY"; // test_1
    //elem.first = "ESLPUXKJQCRIWAOFVBYDNTMHGZ"; // test_4
    //elem.first = "PKNCIDXLAQVWYOSGBFRZUJHMTE"; // test_5
    //elem.first = "RSPWQJBCOXIYMFVHLGKTZNAUED"; // test_6
    //elem.second = fitnessSet(elem.first);
    //population.push_back(elem);
    
    randPopulation();
    sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });
    for_each(population.begin(), population.end(), [this](auto it) { cout << this->translate(it.first) << " " << it.second << endl; }); cout << endl << endl;// cout << i << endl; 

    std::cout << "Message Loaded" << std::endl;
    std::pair<std::string, double> bestCipher("",-100);

    // Run this as long as is defined charsIn the GA
    // constructor
    int gen = 1;
    while (gen < generations) {
        std::vector<std::pair<std::string,double>> interPop;
        std::vector<std::pair<std::string,double>> nextPop;
        std::vector<std::pair<std::string,double>> normPop;

        // SORT POPULATION
        sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });
        

        // FIND BEST
        if (population[0].second > bestCipher.second) {
            bestCipher = population[0];
            cout << "Gen: " << gen << endl;
            cout << "Code Key: " << alphabet << endl;
            cout << "New Best: " << bestCipher.first << std::endl << "Fitnes: " << bestCipher.second << std::endl;
            cout << translate(bestCipher.first) << endl << endl;
            //vector<string> charsIn;
            //for_each(population.begin(), population.end(), [&charsIn](auto it) { if (find(charsIn.begin(), charsIn.end(), it.first) == charsIn.end()) charsIn.push_back(it.first); });
            //<< "Unique Chromosomes: " << charsIn.size() << std::endl;
        }


        // NORMALIZE
        std::pair<std::string,double> elem;
        for (auto it = population.begin(); it != population.end(); it++) {
            elem.first = it->first;
            elem.second =  normalize(it->second, population[0].second, population.back().second);
            normPop.push_back(elem);
        }
        //cout << "norm" << endl; for_each(normPop.begin(), normPop.end(), [](auto it) { cout << it.first << " " << it.second << endl; });


        // BUILD INTERMEDIATE POPULATION   
        for (int i = 0; interPop.size() < popSize; i++, i %= popSize)
            if (dist(engine) <= normPop[i].second)
                interPop.push_back(population[i]);
        
        //for_each(interPop.begin(), interPop.end(), [this](auto it) { cout << this->translate(it.first) << " " << it.second << endl; }); cout << endl << endl;
        
    
        // ELITISM
        if (elitism)
            for (int i = 0; i < elitismCopyAmount; i++)
                nextPop.push_back(interPop[i]);
    


        // CROSSOVER
        while (interPop.empty() != 1) {
  
            // crossPoints -> indices where crossover will occur
            // partnerIndex -> index of second genetor
            vector<int> crossPoints;
            crossPoints.push_back((dist(engine) * (24 / crossovers)) + 1);
            for (int i = 1; i < crossovers; i++)
                crossPoints.push_back((dist(engine) * (23 - (crossPoints[i-1]))) + crossPoints[i-1] + 1);
            
            int partnerIndex = (dist(engine) * (interPop.size() - 1) + 1);


            // genetor A -> always at the first index charsIn the intermediate population
            // genetor B -> at a random index (not 0) charsIn the intermediate population
            string genitorA = interPop[0].first;
            string genitorB = interPop[partnerIndex].first;


            // first section of crossover
            string crossA = genitorA.substr(0, crossPoints[0]);
            string crossB = genitorB.substr(0, crossPoints[0]);

            if (crossovers == 1) {
                singleCrossover(crossA, crossPoints[0], genitorB);
                singleCrossover(crossB, crossPoints[0], genitorA);
            }
            else if (crossovers == 2) {
                doubleCrossover(crossA, crossPoints, genitorA, genitorB);
                doubleCrossover(crossB, crossPoints, genitorB, genitorA);
            }
            else {
                cout << "The only available crossovers are single and double." << endl;
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
            pair<string,double> elemA (crossA, fitnessSet(crossA));
            pair<string,double> elemB (crossB, fitnessSet(crossB));

            // mutate them
            mutate(elemA);
            mutate(elemB);

            // ad to new pop
            nextPop.push_back(elemA);
            nextPop.push_back(elemB);
        }
        population = nextPop;
        gen++;
        if (!(gen % 200))
            cout << gen << endl;
    }
    cout << bestCipher.first << " " << bestCipher.second <<  " " << translate(bestCipher.first) << endl;
}
