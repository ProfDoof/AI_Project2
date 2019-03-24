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

        vector<pair<string,double>> encodedFreq;


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
        while (getline(fin, line) && i < 20)
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


    for (int i = 0; i < codedMessage.size(); i++) {

        if (find(encodedFreq.begin(), encodedFreq.end(), codedMessage[i]) == encodedFreq.end()) {
            count(codedMessage.begin(), codedMessage.end(), codedMessage[i]);
            pair<string, double> elem;
            elem.first = codedMessage[i];
            elem.second = count(codedMessage.begin(), codedMessage.end(), codedMessage[i]) / codedMessage.size();
            encodedFreq.push_back(elem);
        }
    }
    for (auto it = encodedFreq.begin(); it != encodedFreq.end(); it++) {
        cout << it->first << " " << it->second << endl;
    } 
    cout << endl;


    fin.close();
}

void GACipher::randPopulation()
{
    while (population.size() < popSize) {
        std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        std::pair<std::string, double> chromosome;

        // generate cipher key
        for (int i = 0; i < 26; i++) {
            int index = rand() % chars.size();
            chromosome.first += chars[index];
            chars.erase(index, 1);
        }

        chromosome.second = fitnessSet(chromosome.first);
        population.push_back(chromosome);
    }
}



// consider double (like ee, oo, nn, rr, ll)
// conider how often a vowel shows

bool prune(string key, string translated) {

    vector<string> improbable_doubles = { "WW", "AA", "II", "BB", "UU", "ZZ", "KK", "XX", "VV", "JJ", "QQ" };
    
    for (int i = 1; i < translated.size(); i++) {
        if (translated[i-1] == translated[i] && find(improbable_doubles.begin(), improbable_doubles.end(), translated.substr(i-1,2)) != improbable_doubles.end()) {
            return false;
        }
    }


    if (translated.back() == 'V' || translated.back() == 'J') {
        return false;
    }


    return true;
}

// run methods
double GACipher::fitnessSet(std::string cipherValue)
{
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
    vector<pair<string,double>> sortedUniFreq;

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

    exit(EXIT_SUCCESS);

    // UNI FIT END


    // DI FIT START

   /// get the sum of the frequencies that are in diGraph
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
    }

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

    if (diError > 2) {
        diFit /= 5;
    }

    // DI FIT END 


    // TRI FIT START

    // get the sum of the frequencies that are in diGraph
    sum = 0;
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

    // calculate triError (|percievedFreq - observedFreq|)
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
    }
    

    // calculate triFit
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

    //if (triError > 0) {
    //   cout << "tr" << "  " << triFit << endl;
    //}



    // TRI FIT END
    //cout << uniFit << "  " << diFit * 1.5 << "  " << triFit * 5 << "   " << translated << endl;

    if (!prune(cipherValue, translated)) {
        //cout << "A" << endl;
        return 0; //(uniFit * .1);
    }
    return (uniFit + (diFit * 1.5) + (triFit * 5));
   return uniFit;
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


std::string GACipher::translate(std::string cipherValue)
{
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
    //cout << codedMessageChars << endl;
    pair<string,double> elem;
    //elem.first = "GVBMOUSDKWTLIEHFANCY"; // test_1 (broken now)
    //elem.first = "PKNCIDXLAQVWYOSGBFRZUJHMTE"; // test_5
    //elem.first = "RSPWQJBCOXIYMFVHLGKTZNAUED"; // test_6
    //elem.first = "KEWZIARCUHPMGYJLDOVNFTSXQB";
    //elem.second = fitnessSet(elem.first);
    //cout << translate(elem.first) << endl;;
    //population.push_back(elem);
    randPopulation();
    sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });
    for_each(population.begin(), population.end(), [this](auto it) { cout << this->translate(it.first) << " " << it.second << endl; }); cout << endl << endl;// cout << i << endl; 
   // exit(EXIT_SUCCESS);

    // an e got switche to a z this porb happen in crosssover, make map and have coxxover chose new letter based on ccurenced in trans

    

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
    while (gen < 30000)
    {
        // declare our intermediate population
        // and our new population.
        std::vector<std::pair<std::string,double>> intermediatePopulation;
        std::vector<std::pair<std::string,double>> newPopulation;
        std::vector<std::pair<std::string,double>> normPopulation;

        // Sort our population by fitness.
        sort(population.begin(), population.end(), [](const std::pair<std::string, double> a, const std::pair<std::string, double> b) { return a.second > b.second; });

        
          if (gen % 150 == 0) {
            cout << gen << endl;
            cout << translate(population[0].first) << " " << population[0].second << endl << translate(population[1].first) << " " << population[1].second << endl <<  translate(population[2].first) << " " << population[2].second << endl << endl; 
        }
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
            cout << "Code Key: " << cipherKey << endl;
            //vector<string> in;
            //for_each(population.begin(), population.end(), [&in](auto it) { if (find(in.begin(), in.end(), it.first) == in.end()) in.push_back(it.first); });
            std::cout << "New Best: " << bestCipher.first << std::endl << "Fitnes: " << bestCipher.second << std::endl; //<< "Unique Chromosomes: " << in.size() << std::endl;
            cout << translate(bestCipher.first) << endl << endl;;
            //cout << "THEQUICKFOXJUMPSOVERTHELAZYDOGITISCOMMONLYUSEDFORTOUCHTYPINGPRACTICEITISALSOUSEDTOTESTTYPEWRITERSANDCOMPUTERKEYBOARDSSHOWFONTSANDOTHERAPPLICATIONSINVOLVINGALLOFTHELETTERSINTHEENGLISHALPHABETOWINGTOITSBREVITYANDCOHERENCEITHASBECOMEWIDELYKNOWN" << endl << endl;
            //cout << "YWEOEOYWZHNCPZOYYZOYDIOZLZWIKZIYYWZYEXZEIXXIBECPYWEOEYEODUSSZCYHMYLNRNSYMOZKZCICFEIXYESZFYWZLZIYWZSXICOIMOYWIYOUXXZSEODNXECPEIXCNYZGDEYZFOASECPEOXMRIKNSEYZOZIONC"  << endl << "THISISTHELONGESTTESTCASEWEHAVEATTHETIMEIAMMAKINGTHISITISCURRENTLYTWOFORTYSEVENANDIAMTIREDTHEWEATHERMANSAYSTHATSUMMERISCOMINGIAMNOTEXCITEDSPRINGISMYFAVORITESEASON" << endl << endl; // test 5
            //cout << "THEDAYSGROWLONGASISITINMYBEDIAMTIREDANDSTILLSOMEDAYITHINKIWILLESCAPESOFTWAREENGINEERINGTHATISAROUGHCLASSLORDHELPMEGETTHROUGHCOLLEGE" << endl << endl; // test_6
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


        // elitism
        for (int i = 0; i < 10; i++) {
            mutate(intermediatePopulation[i]);
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
                newPopulation.push_back(intermediatePopulation[0]);
                intermediatePopulation.erase(intermediatePopulation.begin());
            }
            else {
                // crossOverIndex : index where crossover occurs (1 - size of cipherKey)
                // partnerIndex : index of second genetor in intermediatePopulation (1 - size of intermediatePopulation)
                int crossOverIndex = rand() % (24) + 1;
                int partnerIndex = rand() % (intermediatePopulation.size() - 1) + 1;

                std::string cross1;
                std::string cross2;

                // two genetors
                // genetor A will always be the first index in the intermediatePopulation
                // genetor B is the chromosome at partnerIndex in the intemediatePopulation
                std::pair<std::string, double> chromoA = intermediatePopulation[0];
                std::pair<std::string, double> chromoB = intermediatePopulation[partnerIndex];

                // populate two new strings with first section of crossover (index 0 - crosOverIndex)
                cross1 = chromoA.first.substr(0,crossOverIndex);
                cross2 = chromoB.first.substr(0,crossOverIndex);

                //cout << crossOverIndex << endl;
                
                // cross1 second half
                for (int i = crossOverIndex; i < (26); i++) {

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
                mutate(chromoA);
                mutate(chromoB);

                chromoA.second = fitnessSet(chromoA.first);
                chromoB.second = fitnessSet(chromoB.first);
                
      
                
                // ad to new pop
                newPopulation.push_back(chromoA);
                newPopulation.push_back(chromoB);
            }
            population = newPopulation;
        }
        gen++;
        //cout << gen << endl;
    }
    cout << bestCipher.first << " " << bestCipher.second <<  " " << translate(bestCipher.first) << endl;
}
