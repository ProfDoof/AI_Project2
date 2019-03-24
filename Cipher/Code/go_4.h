/*
pruned
add best every 100
single mutaion (.05) every 100
elitism
unierr * 1
dierr * 1.5
trifit * 3
*/
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
        // Adjustable Knobs for the Genetic Algorithm
        double copyRate;
        double mutationRate;
        double timeToRun;
        int popSize;
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
        std::string codedMessage;
        std::string cipherKey = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
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
        void loadCodedMessage(std::string filename);
        void loadFreq();
        void randPopulation();
        void decode(std::string cipher);
        std::string translate(std::string key);

        // run methods
        double fitnessSet(std::string cipher);
        void catastrophicMutation(std::string lastString);
        bool mutate(std::pair<std::string, double>& ttm);
        bool mutate(std::pair<std::string, double>& elem, double per);
        void printFitness();

        // post-run methods

        // the actual method "run"
        void run(std::string filename);
};

// Constructor
GACipher::GACipher(double cR, double mR, double ttr, int pS) : copyRate(cR), mutationRate(mR), timeToRun(ttr), popSize(pS) { }

// Destructor
GACipher::~GACipher() { }

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
    getline(fin, codedMessage);
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

    for (int i = 0; i < codedMessage.size(); i++) {
        int index = cipherKey.find(codedMessage[i]);
        output += key[index];
    }
    return output;
}

// INCOMPLETE (im sure there is more pruning to be done)
bool prune(string key, string translated) {
    vector<string> improbable_doubles = { "WW", "AA", "II", "BB", "UU", "ZZ", "KK", "XX", "VV", "JJ", "QQ" };
    
    for (int i = 1; i < translated.size(); i++) {
        if (translated[i-1] == translated[i] && find(improbable_doubles.begin(), improbable_doubles.end(), translated.substr(i-1,2)) != improbable_doubles.end())
            return false;

        if (translated[i] == 'Q' && i < 25 && translated[i+1] != 'U')
            return false;

    }
        

    if (translated.back() == 'V' || translated.back() == 'J' || translated.back() == 'Q')
        return false;

    return true;
}

// INCOMPLETE (this might need to return nothing)
bool GACipher::mutate(std::pair<std::string, double>& elem) {
    bool mutated = false;
    for (int i = 0; i < 26; i++) {
        if (dist(engine) <= mutationRate) {
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

// INCOMPLETE (this might need to return nothing)
bool GACipher::mutate(std::pair<std::string, double>& elem, double per) {
    bool mutated = false;
    for (int i = 0; i < 26; i++) {
        if (dist(engine) <= per) {
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
    if (!prune(key, translated))
        factor *= 5;

    //cout << uniError << " " << diError << " " << 3 * triFit << endl;
    //cout << 5 - (factor * (uniError + (1.5 * diError))) + triFit << endl;
    //exit(EXIT_SUCCESS);

    return 5 - (factor * (uniError + (1.5 * diError))) + (3 * triFit);
}






// post-run methods

// the method "run"
void GACipher::run(std::string filename)
{
    setMutationRate(.05);
    std::cout << "Start Run" << std::endl;
    srand(time(NULL));
    loadCodedMessage(filename);
    loadFreq();

    pair<string,double> elem;
    //elem.first = "PKNCIDXLAQVWYOSGBFRZUJHMTE"; // test_5
    //elem.first = "RSPWQJBCOXIYMFVHLGKTZNAUED"; // test_6

    //elem.second = fitnessSet(elem.first);
    //population.push_back(elem);

    //elem.first = "KWNGIFXVAZCBDSOUMPRJYQHLTE"; // test_5
    //elem.second = fitnessSet(elem.first);
    //population.push_back(elem);
    
    randPopulation();
    sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });
    //for_each(population.begin(), population.end(), [this](auto it) { cout << this->translate(it.first) << " " << it.second << endl; }); cout << endl << endl;// cout << i << endl; 
    //exit(EXIT_SUCCESS);
    std::cout << "Message Loaded" << std::endl;


    std::pair<std::string, double> bestCipher("",-100);

    // Run this as long as is defined in the GA
    // constructor
    int gen = 1;
    while (gen < 30000)
    {
        // declare our intermediate population
        // and our new population.
        std::vector<std::pair<std::string,double>> intermediatePopulation;
        std::vector<std::pair<std::string,double>> newPopulation;
        std::vector<std::pair<std::string,double>> normPopulation;

        if (gen % 100 == 99) {
            pair<string,double> elem = bestCipher;
            mutate(elem, 1);
            population.push_back(elem);
        }
        

        // SORT POPULATION (by fitness)
        sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });

       
        if (gen % 500 == 0) {
            cout << gen << endl;
            //for_each(population.begin(), population.end(), [this](auto it) { cout << this->translate(it.first) << " " << it.second << endl; }); cout << endl << endl;
        }



        // FIND BEST
        if (population[0].second > bestCipher.second)
        {
            bestCipher = population[0];
            cout << "go_1" << endl;
            cout << "Gen: " << gen << endl;
            cout << "Code Key: " << cipherKey << endl;
            vector<string> in;
            for_each(population.begin(), population.end(), [&in](auto it) { if (find(in.begin(), in.end(), it.first) == in.end()) in.push_back(it.first); });
            std::cout << "New Best: " << bestCipher.first << std::endl << "Fitnes: " << bestCipher.second << " " << "Unique Chromosomes: " << in.size() << " " << population.size() << std::endl;
            cout << translate(bestCipher.first) << endl << endl;
        }


        // normalize current population
        for (auto it = population.begin(); it != population.end(); it++) {
            std::pair<std::string,double> normPair;
            double normFit = (((0.95 - 0.05) * (it->second - population.back().second)) / (population[0].second - population.back().second)) + 0.05;
            normPair.first = it->first;
            normPair.second = normFit;
            normPopulation.push_back(normPair);
        }
        //cout << "norm" << endl; for_each(normPopulation.begin(), normPopulation.end(), [](auto it) { cout << it.first << " " << it.second << endl; });
       
        // Build our intermediate population.
        for (int i = 0; intermediatePopulation.size() < popSize; i++, i %= popSize) {
            if (dist(engine) <= normPopulation[i].second) {
                std::pair<std::string, double> temp (population[i]);
                intermediatePopulation.push_back(temp);
            }
		}
        

        // elitism
        for (int i = 0; i < 10; i++) {
            //mutate(intermediatePopulation[i]);
            intermediatePopulation[i].second = fitnessSet(intermediatePopulation[i].first);
            newPopulation.push_back(intermediatePopulation[i]);
            intermediatePopulation.erase(intermediatePopulation.begin());
        }

        // Generate our new population.
        while (!intermediatePopulation.empty())
        {
            // copyRate% chance of being pushed
            // directly into the next gen.
            if (dist(engine) < copyRate || intermediatePopulation.size() == 1) {
                if (mutate(intermediatePopulation[0])) {
                    intermediatePopulation[0].second = fitnessSet(intermediatePopulation[0].first);
                    newPopulation.push_back(intermediatePopulation[0]);
                    intermediatePopulation.erase(intermediatePopulation.begin());
                }
            }
            else {
                // crossOverIndex : index where crossover occurs (1 - size of cipherKey)
                // partnerIndex : index of second genetor in intermediatePopulation (1 - size of intermediatePopulation)
                //int crossOverIndex = rand() % (24) + 1;
                int crossOverIndex = (dist(engine) * 24) + 1;
                //int partnerIndex = rand() % (intermediatePopulation.size() - 1) + 1;
                int partnerIndex = (dist(engine) * (intermediatePopulation.size() - 1) + 1);
        

                std::string cross1;
                std::string cross2;

                // two genetors
                // genetor A will always be the first index in the intermediatePopulation
                // genetor B is the chromosome at partnerIndex in the intemediatePopulation
                std::pair<std::string, double> chromoA;
                std::pair<std::string, double> chromoB;

                chromoA = intermediatePopulation[0];
                chromoB = intermediatePopulation[partnerIndex];
        
                // populate two new strings with first section of crossover (index 0 - crosOverIndex)
                cross1 = chromoA.first.substr(0,crossOverIndex);
                cross2 = chromoB.first.substr(0,crossOverIndex);

                //cout << crossOverIndex << endl;

                
                // cross1 second half
                for (int i = crossOverIndex; i < 26; i++) {

                    // if the char being considered is not in the first half of the cross over then we can add it
                    //      to the second half of the crossover
                    if (cross1.find(chromoB.first[i]) == string::npos) {
                        cross1 += chromoB.first[i];
                    }

                    // if the char being considered is in the first half of the crossover replace it with a valid, random char
                    else {
                        // str : the string in the other chromosome (B) to be crossed over into crossed 1
                        // we do not want to choose a character that is to come
                        string str = cross1 + chromoB.first.substr(crossOverIndex, 25 - crossOverIndex);
                        for (int k = 0; k < 26; k++) {
                            string chr = uniFreq[k].first; 
                            if (str.find(chr) == string::npos) {
                                cross1 += chr;
                                //cout << "a";
                                break;
                            }
                        }
                    }
                }

                // cross2 second hal
                for (int i = crossOverIndex; i < 26; i++) {

                    // if the char being considered is not in the first half of the cross over then we can add it
                    //      to the second half of the crossover
                    if (cross2.find(chromoA.first[i]) == string::npos) {
                        cross2 += chromoA.first[i];
                    }

                    // if the char being considered is in the first half of the crossover replace it with a valid, random char
                    else {
                        // str : the string in the other chromosome (A) to be crossed over into crossed 2
                        // we do not want to choose a character that is to come
                        string str = cross2 + chromoA.first.substr(crossOverIndex, 25 - crossOverIndex);
                        for (int k = 0; k < 26; k++) {
                            string chr = uniFreq[k].first; 
                            if (str.find(chr) == string::npos) {
                                cross2 += chr;
                                // cout << "a";
                                break;
                            }
                        }
                    }
                }

                
                
                /*
                cout << crossOverIndex << endl;
                cout << chromoA.first.substr(0, crossOverIndex) << "  " << chromoA.first.substr(crossOverIndex, 26 - crossOverIndex)  << endl;
                cout << chromoB.first.substr(0, crossOverIndex) << "  " << chromoB.first.substr(crossOverIndex, 26 - crossOverIndex)  << endl;
                cout << cross1.substr(0, crossOverIndex) << "  " << cross1.substr(crossOverIndex, 26 - crossOverIndex)  << endl;
                cout << cross2.substr(0, crossOverIndex) << "  " << cross2.substr(crossOverIndex, 26 - crossOverIndex)  << endl;
                */
    
                // remove genetors from intermediatePopulation
                intermediatePopulation.erase(intermediatePopulation.begin()+partnerIndex);
                intermediatePopulation.erase(intermediatePopulation.begin());

                // create pairs so cross1/cross2 can be added to population
                chromoA.first = cross1;
                chromoB.first = cross2;

                // mutate them
                if (mutate(chromoA)) chromoA.second = fitnessSet(chromoA.first);
                if (mutate(chromoB)) chromoB.second = fitnessSet(chromoB.first);
                
                // add to new pop
                newPopulation.push_back(chromoA);
                newPopulation.push_back(chromoB);
               
            }
            population = newPopulation;
        }
        gen++;


    }
    cout << bestCipher.first << " " << bestCipher.second <<  " " << translate(bestCipher.first) << endl;
}
