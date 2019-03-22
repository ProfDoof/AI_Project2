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


        std::vector<std::pair<std::string,double>> uniFreq;
        std::vector<std::pair<std::string,double>> diFreq;
        std::vector<std::pair<std::string,double>> triFreq;


        // Coded Message and Population
        std::string codedMessage;
        std::string codedMessageChars;
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
        std::string translate(std::string cipherValue);

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

GACipher::GACipher(double cR, double mR, double ttr, int pS) : copyRate(cR), mutationRate(mR), timeToRun(ttr), popSize(pS)
{

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

    fin.open("../Frequency/Frequency_1.txt");

    if (fin.is_open())
    {
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

    //std::ifstream fin;
    //std::string line;

    fin.open("../Frequency/Frequency_2.txt");

    if (fin.is_open())
    {
        int i = 0;
        while (getline(fin, line) && i < 30)
        {
            std::pair<std::string, double> temp;
            std::stringstream ss(line);
            ss >> temp.first >> temp.second;
            diFreq.push_back(temp);
            diFreqMap.insert(temp);
            i++;
        }
    }

    fin.close();

    //std::ifstream fin;
    //std::string line;

    fin.open("../Frequency/Frequency_3.txt");

    if (fin.is_open())
    {
        int i = 0;
        while (getline(fin, line) && i < 20)
        {
            std::pair<std::string, double> temp;
            std::stringstream ss(line);
            ss >> temp.first >> temp.second;
            triFreq.push_back(temp);
            triFreqMap.insert(temp);
            i++;
        }
    }

    fin.close();
}

void GACipher::loadCodedMessage(std::string filename)
{
    // codedMessage <- (file contents)
    std::ifstream fin;
    fin.open(filename);
    getline(fin, codedMessage);
    fin.close();

    // codedMessageChars is a string which represents the set of all chars in the codedMessage
    for (int i = 0; i < codedMessage.size(); i++) {
        if (codedMessageChars.find(codedMessage[i]) == std::string::npos)
            codedMessageChars += codedMessage[i];
    }

    sort(codedMessageChars.begin(), codedMessageChars.end());
}

void GACipher::randPopulation()
{
    while (population.size() < popSize) {
        std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::pair<std::string, double> chromosome;

        // generate cipher key
        for (int i = 0; i < codedMessageChars.size(); i++) {
            int index = rand() % chars.size();
            chromosome.first += chars[index];
            chars.erase(index, 1);
        }

        chromosome.second = fitnessSet(chromosome.first);
        population.push_back(chromosome);
    }
}



// run methods
double GACipher::fitnessSet(std::string cipherValue)
{
    std::string cipherKey = codedMessageChars;
    std::string translated = translate(cipherValue);
    
    double sum = 0;
    double uniFit = 0;
    double uniError = 0;
    double diFit = 0;
    double diError = 0;
    double triFit = 0;
    double triError = 0;

    map<char, double> perUniFreq;
    map<string, double> perDiFreq;
    map<string, double> perTriFreq;

    // UNI FIT START

    // get sum of frequencies that are in uni graph
    for (int i = 0; i < cipherValue.size(); i++) {
        sum += uniFreqMap[cipherValue[i]];
    }

    // create a percieved freq (frequency when only chars that are in key are considered)
    std::pair<char, double> elemUni;
    for (int i = 0; i < cipherValue.size(); i++) {
        elemUni.first = cipherValue[i];
        elemUni.second = uniFreqMap[cipherValue[i]] / sum;
        perUniFreq.insert(elemUni);
    }

    // calculate uniError (|percievedFreq - observedFreq|)
    for (int i = 0; i < cipherValue.size(); i++) {

        // count the number of occurances a character (of cipher value) in the translated text
        int occurances = count(translated.begin(), translated.end(), (char) cipherValue[i]);

        // find the frequency by dividing the number of occurances by the length of the the string
        double frequency = (1.0 * occurances) / translated.size();

        // uniError is expected frequency - observed frequency
        uniError += abs(perUniFreq[cipherValue[i]] - frequency);
    }

    // calculate uniFit
    for (int i = 0; i < cipherValue.size(); i++) {

        // str : char whose fitness is being measured
        string str = cipherValue.substr(i,1);
        
        // it_index = the iterator's locatation in uniFreq (vector<string, double>) where the char whose freq is being measured is
        auto it_index = find_if(uniFreq.begin(), uniFreq.end(), [str](auto it_1) { return str == it_1.first.c_str(); });

        // it_index's numerical location/index
        int index = (it_index - uniFreq.begin()) + 1;
    
        // UNI_FREQ : places emphasis on how frequently a character appears in the translated text. the number of apppearences is
        //            divided by index (frequency based on freq charts)
        uniFit += ((1.0 * count(translated.begin(), translated.end(), (char) cipherValue[i]) / translated.size()) / index);
    }

    // if the error is high, drastically lower uniFreq
    if (uniError > .5) {
        uniFit /= 2;
    }

    // UNI FIT END


    /*// DI FIT START

    // get the sum of the frequencies that are in diGraph
    sum = 0;
    for (int i = 1; i < translated.size(); i++) {
        sum += diFreqMap[translated.substr(i-1,2)];
    }

    // create a percieved freq (frequency when only chars that are in key are considered)
    std::pair<string, double> elemDi;
    for (int i = 1; i < translated.size(); i++) {
        string str = translated.substr(i - 1, 2);
        elemDi.first = str;
        elemDi.second = diFreqMap[str] / sum;
        perDiFreq.insert(elemDi);
    }

    // calculate diError (|percievedFreq - observedFreq|)
    for (int i = 1; i < translated.size(); i++) {
        
        // current substring
        string str = translated.substr(i - 1, 2);

        // count the number of occurances a string in the translated text
        int count = 0;
        for (int k = i; k < translated.size(); k++) {
            if (translated.substr(k - 1, 2) == str) {
                count += 1;
            }
        }

        // find the frequency by dividing the number of occurances by the length of the the string
        double frequency = (1.0 * count) / translated.size();

        // diError is expected frequency - observed frequency
        diError += abs(perDiFreq[translated.substr(i - 1, 2)] - frequency);
    }*/

    // calculate diFit
    set<string> alreadyCounted;
    for (int i = 1; i < translated.size(); i++) {

        // str : char whose fitness is being measured
        string str = translated.substr(i - 1, 2);

        // substr has already been considered in diFit calculations, we dont need to calculate that substring's fitness again
        if (alreadyCounted.find(str) != alreadyCounted.end()) {
            continue;
        }
        
        // it_index = the iterator's locatation in uniFreq (vector<string, double>) where the char whose freq is being measured is
        auto it_index = find_if(diFreq.begin(), diFreq.end(), [str](auto it) { return str == it.first; });
        
        // index = it_index's numerical location/index in diFreq, it is 0 if the substring was not found
        // so if it is in freqDif we contiue, it if is not we don't care 
        if (it_index != diFreq.end()) {
            int index = (it_index - diFreq.begin()) + 1;

            // find the number of occurences os substring in translated text
            int count = 0;
            for (int k = i; k < translated.size(); k++) {
                if (translated.substr(k - 1, 2) == str) {
                    count += 1;
                } 
            }

            diFit += ((1.0 * count) / (translated.size() - 1)) / index;
            //cout << str << " " << ((1.0 * count) / (translated.size() - 1)) / index << endl;
            
            // add to set (allows to not have any repeating substrings count toward diFit)
            alreadyCounted.insert(str);
        }
    }

    /*if (diError > 2) {
        diFit /= 5;
    }*/

    // DI FIT END 


    // TRI FIT START

    // get the sum of the frequencies that are in diGraph
   /* sum = 0;
    for (int i = 2; i < translated.size(); i++) {
        sum += triFreqMap[translated.substr(i - 2, 3)];
    }

    // create a percieved freq (frequency when only chars that are in key are considered)
    std::pair<string, double> elemTri;
    for (int i = 2; i < translated.size(); i++) {
        string str = translated.substr(i - 2, 3);
        elemTri.first = str;
        elemTri.second = triFreqMap[str] / sum;
        perTriFreq.insert(elemTri);
    }

    // calculate diError (|percievedFreq - observedFreq|)
    for (int i = 2; i < translated.size(); i++) {
        
        // current substring
        string str = translated.substr(i - 2, 3);

        // count the number of occurances a string in the translated text
        int count = 0;
        for (int k = i; k < translated.size(); k++) {
            if (translated.substr(k - 2, 3) == str) {
                count += 1;
            }
        }

        // find the frequency by dividing the number of occurances by the length of the the string
        double frequency = (1.0 * count) / translated.size();

        // diError is expected frequency - observed frequency
        triError += abs(perTriFreq[translated.substr(i - 2, 3)] - frequency);
    }*/

    // calculate diFit
    alreadyCounted.clear();
    for (int i = 2; i < translated.size(); i++) {

        // str : char whose fitness is being measured
        string str = translated.substr(i - 2, 3);

        // substr has already been considered in diFit calculations, we dont need to calculate that substring's fitness again
        if (alreadyCounted.find(str) != alreadyCounted.end()) {
            continue;
        }
        
        // it_index = the iterator's locatation in uniFreq (vector<string, double>) where the char whose freq is being measured is
        auto it_index = find_if(triFreq.begin(), triFreq.end(), [str](auto it) { return str == it.first; });
        
        // index = it_index's numerical location/index in diFreq, it is 0 if the substring was not found
        // so if it is in freqDif we contiue, it if is not we don't care 
        if (it_index != triFreq.end()) {
            int index = (it_index - triFreq.begin()) + 1;

            // find the number of occurences os substring in translated text
            int count = 0;
            for (int k = i; k < translated.size(); k++) {
                if (translated.substr(k - 2, 3) == str) {
                    count += 1;
                } 
            }

            triFit += ((1.0 * count) / (translated.size() - 2)) / index;
            //cout << str << " " << ((1.0 * count) / (translated.size() - 1)) / index << endl;
            
            // add to set (allows to not have any repeating substrings count toward diFit)
            alreadyCounted.insert(str);
        }
    }

    /*if (triFit > 0) {
       cout << "tr" << "  " << triFit << endl;
    }*/



    // TRI FIT END

   return uniFit + (diFit * 1.5) + (triFit * 5);
   //return uniFit;
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

std::string GACipher::translate(std::string cipherValue)
{
    std::string cipherKey = codedMessageChars;
    string output = "";

    for (int i = 0; i < codedMessage.size(); i++) {
        int index = cipherKey.find(codedMessage[i]);
        output += cipherValue[index];
    }

    return output;
}


// post-run methods

// the method "run"
void GACipher::run(std::string filename)
{
    srand(time(NULL));
    std::cout << "Start Run" << std::endl;

    // codeMessage <- (string in file)
    loadCodedMessage(filename);
    // cout << codedMessage << endl;
    // cout << codedMessageChars << endl;


    // freqChart <- (frequencies in file)
    loadFreq();
    // for_each(freqChart.begin(), freqChart.end(), [](auto it) { cout << it.first << " " << it.second << endl; });

    // population <- (random cipher keys)
    //randPopulation();
    //for_each(population.begin(), population.end(), [](auto it) { cout << it.first << " " << it.second << endl; });
    cout << codedMessageChars << endl;
    pair<string,double> elem;
    elem.first = "LEKBRTUAHDONISPWGMC";
    elem.second = fitnessSet(elem.first);
    population.push_back(elem);
    randPopulation();
    sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });
    for_each(population.begin(), population.end(), [](auto it) { cout << it.first << " " << it.second << endl; }); cout << endl << endl;// cout << i << endl; 
   // exit(EXIT_SUCCESS);


    

    std::cout << "Message Loaded" << std::endl;
    // printFitness();

    // Declare the timer variables
    //auto startTime = std::chrono::high_resolution_clock::now();
    //auto currentTime = startTime;
    //std::chrono::duration<double, std::milli> compare = currentTime - startTime;
    std::pair<std::string, double> bestCipher("",0);

    // Run this as long as is defined in the GA
    // constructor
    int gen = 1;
    while (gen < 10000)
    {
        // declare our intermediate population
        // and our new population.
        std::vector<std::pair<std::string,double>> intermediatePopulation;
        std::vector<std::pair<std::string,double>> newPopulation;
        std::vector<std::pair<std::string,double>> normPopulation;

        // Sort our population by fitness.
        sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });

        
        
        //cout << population.size() << endl;
        double i = 0;
        //cout << population[0].first << " " << population[0].second << endl;
        //for_each(population.begin(), population.end(), [&i](auto it) { cout << it.first << " " << it.second << endl;  i += it.second; }); cout << endl << endl;// cout << i << endl; 


        // Check and see if we have found a better
        // fitting cipher than our current best
        // in this case lower is better (it's like
        // golf)
        if (population[0].second > bestCipher.second)
        {
            bestCipher = population[0];
            cout << "Gen: " << gen << endl;
            cout <<      "Code Key: " << codedMessageChars << endl;
            //vector<string> in;
            //for_each(population.begin(), population.end(), [&in](auto it) { if (find(in.begin(), in.end(), it.first) == in.end()) in.push_back(it.first); });
            std::cout << "New Best: " << bestCipher.first << std::endl << "Fitnes: " << bestCipher.second << std::endl; //<< "Unique Chromosomes: " << in.size() << std::endl;
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
        for (int i = 0; intermediatePopulation.size() < popSize; i++, i %= popSize)
        {
            // ???????????? This needs work to work
            // correctly because the fitness is no
            // longer between 0 and 1 for certain.
            //if (dist(engine) <= 1.05-(population[i].second/population.back().second)) 
            if (((double) rand() / (RAND_MAX)) <= normPopulation[i].second) {
                std::pair<std::string, double> temp (population[i]);
                intermediatePopulation.push_back(temp);
            }
		}
        //for_each(intermediatePopulation.begin(), intermediatePopulation.end(), [](auto it) { cout << it.first << " " << it.second << endl; });

        // elitism
        /*for (int i = 0; i < 10; i++) {
            mutate(intermediatePopulation[i]);
            newPopulation.push_back(intermediatePopulation[i]);
            intermediatePopulation.erase(intermediatePopulation.begin());
        }*/

        

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
                //bool flag = mutate(intermediatePopulation[0]);
                // std::cout << intermediatePopulation[0].first << " " << intermediatePopulation[0].second << std::endl;

                // If the pair was mutated we need to
                // recalculate the fitness.
                //if (flag)
                //{
                //    intermediatePopulation[0].second = fitnessSet(intermediatePopulation[0].first);
                //}

                newPopulation.push_back(intermediatePopulation[0]);
                intermediatePopulation.erase(intermediatePopulation.begin());
            }
            // 100-copyRate% chance of crossover
            else
            {
                //int crossOverIndex = dist(engine)*codedMessageChars.size();
                int crossOverIndex = rand() % (codedMessageChars.size() - 1) + 1;
                //int partnerIndex = dist(engine)*intermediatePopulation.size();
                int partnerIndex = rand() % (intermediatePopulation.size() - 1) + 1;
                std::string cross1;
                std::string cross2;

                // If the partnerIndex is not equal to
                // zero then we can crossover. If it is
                // equal to zero we can not crossover
                // so let's just push it directly through
                //if (partnerIndex != 0)
                //{
                // Collect the two pairs to be
                // crossed.
                std::pair<std::string, double> chromoA = intermediatePopulation[0];
                std::pair<std::string, double> chromoB = intermediatePopulation[partnerIndex];
                //cout << crossOverIndex << endl;
                //cout << "pop size " << intermediatePopulation.size() << "   partnerIndex " << partnerIndex << endl;
                //cout << "A: " << chromoA.first << endl;
                //cout << "B: " << chromoB.first << endl << endl;

                // Put in the first part of each
                // set.
                cross1 = chromoA.first.substr(0,crossOverIndex);
                cross2 = chromoB.first.substr(0,crossOverIndex);
                //std::cout << "1 part 1   " << cross1 << std::endl;
                //std::cout << "2 part 1   " << cross2 << std::endl;

                

                // Now iterate through the partner
                // string for both pushing each
                // letter that isn't in the cross
                // into the new cross. if letter is in pick a random letter in aplhabet that not in current chromo (first) (string)

                // WE CAN ADD BIAS TO RANDOM CHARS ADDED BY CREATING VECTOR THAT IS SORTED AS UNI_FREQ IS, for char in this vector if not in chromo.first, add it, this gives
                //  precedence to chars which should appear a lot (AND SHOULD WORK)

                // i want diff to only contian chars that arent in cross1/2 or upcoming in opposite chromo
                for (int i = crossOverIndex; i < chromoB.first.size(); i++) {
                    if (cross1.find(chromoB.first[i]) == std::string::npos) {
                        cross1 += chromoB.first[i];
                    }
                    else {
                        //cout << "crossed 1" << endl;
                        std::vector<char> chars = alphabet;
                        std::set<char> crossChars;

                        // make current chars in cross1 and upcoming chars in opposite chromo into a vector
                        for_each(cross1.begin(), cross1.end(), [&crossChars](auto it) { crossChars.insert(it); });
                        for_each(chromoB.first.begin() + crossOverIndex, chromoB.first.end(), [&crossChars](auto it) { crossChars.insert(it); });
                        //cout << "INDEX  " << *(chromoB.first.begin() + crossOverIndex) << endl;
                        //cout << "CC 1 "; for_each(crossChars.begin(), crossChars.end(), [](auto it) { cout << it << " "; }); cout << endl;

                        // find all chars in alpha bet that are not in cross1, put those in diff
                        vector<char> diff;
                        set_difference(chars.begin(), chars.end(), crossChars.begin(), crossChars.end(), std::inserter(diff, diff.begin()));
                        //for_each(diff.begin(), diff.end(), [](auto it) { cout << it << " "; }); cout << endl;
                        
                        // choose a random char in diff and add it to cross1
                        cross1 += diff[rand() % diff.size()];
                    }
                }

                for (int i = crossOverIndex; i < chromoA.first.size(); i++) {
                    if (cross2.find(chromoA.first[i]) == std::string::npos) {
                        cross2 += chromoA.first[i];
                    }
                    else {
                        //cout << "crossed 2" << endl;
                        std::vector<char> chars = alphabet;
                        std::set<char> crossChars;

                        // make current chars in cross2 and upcoming chars in opposite chromo into a vector
                        for_each(cross2.begin(), cross2.end(), [&crossChars](auto it) { crossChars.insert(it); });
                        for_each(chromoA.first.begin() + crossOverIndex, chromoA.first.end(), [&crossChars](auto it) { crossChars.insert(it); });
                        //cout << "CC 2 "; for_each(crossChars.begin(), crossChars.end(), [](auto it) { cout << it << " "; }); cout << endl;


                        // find all chars in alpha bet that are not in chromoA, put those in diff
                        vector<char> diff;
                        set_difference(chars.begin(), chars.end(), crossChars.begin(), crossChars.end(), std::inserter(diff, diff.begin()));
                        //for_each(diff.begin(), diff.end(), [](auto it) { cout << it << " "; }); cout << endl;
                        
                        cross2 += diff[rand() % diff.size()];
                    }
                }

                /*if (chromoA.first.size() != codedMessageChars.size()) {
                    cout << "A ERROR" << endl;
                    exit(EXIT_FAILURE);
                }
                if (chromoB.first.size() != codedMessageChars.size()) {

                    cout << "B ERROR" << endl;
                    exit(EXIT_FAILURE);
                }*/
                
                intermediatePopulation.erase(intermediatePopulation.begin()+partnerIndex);
                intermediatePopulation.erase(intermediatePopulation.begin());

                //std::cout << "1 final   " << cross1 << std::endl;
                //std::cout << "2 final   " << cross2 << std::endl;

                

                chromoA.first = cross1;
                chromoB.first = cross2;
                chromoA.second = fitnessSet(chromoA.first);
                chromoB.second = fitnessSet(chromoB.first);
                //chromoA.second = 0;
                //chromoB.second = 0;
                mutate(chromoA);
                mutate(chromoB);

                newPopulation.push_back(chromoA);
                newPopulation.push_back(chromoB);
            //}

            }
            //cout << "norm " << endl; for_each(newPopulation.begin(), newPopulation.end(), [](auto it) { cout << it.first << " " << it.second << endl; }); cout << endl << endl;

            //cout << "new " << endl;for_each(newPopulation.begin(), newPopulation.end(), [](auto it) { cout << it.first << " " << it.second << endl; });



            population = newPopulation;
        }

        // Get the current time
        //currentTime = std::chrono::high_resolution_clock::now();
        //compare = currentTime - startTime;
        gen++;
        if (gen % 100 == 0) {
            cout << gen << endl;
            //cout << population[0].first << " " << population[0].second << "   " << population[1].first << " " << population[1].second << "   " <<  population[2].first << " " << population[2].second << endl;        }
        }
    }
    cout << bestCipher.first << " " << bestCipher.second << endl;
}
