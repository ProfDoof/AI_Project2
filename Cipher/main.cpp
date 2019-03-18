#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <vector>
#include <set>
#include <random>
#include <time.h>
#include "Chromosome.h"

using namespace std;


/*
PURPOSE
    get frequencies from a specified file and put them in a map
PARAMETERS 
    filename : name of freq file (includes ".txt")
    size: how many frequencies the dictionary should hold
RETURNS (map<string,double>)
    returns a map of the k,v pairs stored in file
*/
map<string, double> read_file_freq(const string filename, const int size) {
    int i = 0;
    double value;
    string key;
    map<string, double> freq;
    ifstream in_file(filename.c_str());

    while (in_file >> key >> value && i < size) {
        freq[key] = value;
        i++;
    }

    in_file.close();
    return freq;
}

/*
PURPOSE 
    get frequencies loaded into a single vector
PARAMETERS (none)
RETURNS (vector of map<string,double>)
    returns a vector containing 3 maps, eahc map holds the frequencies for 
    uni, di, and trigraphs
*/
vector<map<string, double>> get_freq() {
    vector<map<string, double>> freq;
    freq.push_back(read_file_freq("Frequency/Frequency_1.txt", 26));
    freq.push_back(read_file_freq("Frequency/Frequency_2.txt", 100));
    freq.push_back(read_file_freq("Frequency/Frequency_3.txt", 100));
    return freq;
}

/*
PURPOSE
    get input file (encoded text)
PARAMETERS
    filename : the name of the file which the encoded text is in
RETURNS (string)
    returns the encoded text
*/
string read_file_case(const string filename) {
    string text;
    ifstream in_file(filename.c_str());
    in_file >> text;
    return text;
}

/*
PURPOSE
    generate a random string, this is used to create different keys within chromosomes
PARAMATERS
    size: the size of the string to be generated
RETURNS (string)
    returns a randomly generated string (unique chars)
NOTES
    this could be replaced with simply shuffling a string and choosing n indicies
    i could potentially generate random strings based on unigraph frequency
*/
string random_string_gen(int size) {
    int i = 0;
    int index;
    string str = "";
    string pool = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    while (i < size) {
        index = rand() % pool.size();
        str += pool[index];
        pool.erase(index, 1);
        i++;
    }
    return str;
}



int main() {
    srand(time(NULL));
    set<char> alpha_set;
    string alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < 26; i++) {
        alpha_set.insert((char)alpha[i]);
    }

    // READ IN FREQ DATA //
    vector<map<string, double>> freq = get_freq();
    //for ( int i = 0; i < 3; i++) {
    //    for_each(freq[i].begin(), freq[i].end(), [](auto it) { cout << it.first << " "; }); cout << endl;
    //}

    
    
    
    // READ INPUT FILE //
    string text = read_file_case("Test_Files/test_1.txt");
   

    // GET CHARACTER SET //
    set<char> char_set;
    for_each(text.begin(), text.end(), [&char_set](auto it) { char_set.insert(it); });
    
    // GENERATE POPULATION //
    int pop_size = 50;
    vector<Chromosome> population;

    for (int i = 0; i < pop_size; i++) {
        Chromosome chromo;
        chromo.set_key(char_set, random_string_gen(char_set.size()));
        chromo.set_text(text);
        chromo.set_fitness(freq);
        population.push_back(chromo);
    }

    //for (int i = 0; i < 100; i++) {
    //    cout << population[i].fitness << " " << population[i].text << endl;
    //}


   


    // GENERATION RUN //

    int best_fit = 0;
    int gen = 1;
    while (gen < 100) {
        cout << "GEN: " << gen << endl;
        vector<Chromosome> inter_pop;
        vector<Chromosome> new_pop;

        //cout << population.size() << " ";
        //for (int i = 0; i < pop_size; i++) {
        //    cout << population[i].fitness << " ";
        //}
        //cout << endl << endl;
    
        // SORT POP
        sort(population.begin(), population.end());
        //cout << population.size() << " ";
        //cout << "sorted pop ";
        for (int i = 0; i < pop_size; i++) {
            cout << population[i].fitness << " ";
        }
        cout << endl;
        
        // GET BEST 

        if (population.back().fitness > best_fit) {
            //population.end()->print_key();
            cout << "NEW BEST: " << population.back().text <<  " " << population.back().fitness << endl << endl;
            best_fit = population.back().fitness;
        }

        // NORMALIZE POP
        double min_fit = population.begin()->fitness;
        double max_fit = population.back().fitness;
        vector<double> normalized_fit;
        //cout << "min " << min_fit << "\tmax " << max_fit << endl; 
        for (auto it = population.begin(); it != population.end(); it++) {
            normalized_fit.push_back(((.95 - .05) * ((it->fitness - min_fit) / (max_fit - min_fit))) + 0.05);
        }
        
        //cout << "norm pop ";
        for (int i = 0; i < pop_size; i++) {
            cout << normalized_fit[i] << " ";
        }
        cout << endl << endl;;



    
        // push rest into inter pop
        for (int i = 0; inter_pop.size() < pop_size; i++, i %= population.size()) {
            if (((double) rand() / (RAND_MAX))  <= normalized_fit[i]) {
                inter_pop.push_back(population[i]);
            }
		}

        //for (int i = 0; i < pop_size; i++) {
        //   cout << inter_pop[i].fitness << endl;
        //}

        //cout << inter_pop.size() << endl;
        

        // crossover rest
        while (!inter_pop.empty()) {
            
            // 25% chance of being pushed to new generation
            if (((double) rand() / (RAND_MAX)) <= 0.25 || inter_pop.size() == 1) {
                int i = rand() % population.size();
                new_pop.push_back(population[i]);
                inter_pop.erase(inter_pop.begin() + i);
            } 
            // crossover
            else {
                string crossed_1 = "";
                string crossed_2 = "";
                int parent_B_index = rand() % inter_pop.size();
                Chromosome parent_A = inter_pop[0];
                Chromosome parent_B = inter_pop[parent_B_index];
                int cross_index = rand() % char_set.size();
                auto cross_key = char_set.begin();
                advance(cross_key, cross_index);

                // generate crossed over strings
                for (auto it = char_set.begin(); it != cross_key; it++) {
                    crossed_1 += parent_A.key[*it];
                    crossed_2 += parent_B.key[*it];
                }

                for (auto it = cross_key; it != char_set.end(); it++) {
                    crossed_1 += parent_B.key[*it];
                    crossed_2 += parent_A.key[*it];
                }

                // ensure that crossed is indeed still single substitution
                set<char> cross_1_set;
                for (auto it = crossed_1.begin(); it != crossed_1.end(); it++) {
                    cross_1_set.insert(*it);
                }
                
                vector<char> cross_1_comp;
                set_difference(alpha_set.begin(), alpha_set.end(), cross_1_set.begin(), cross_1_set.end(), inserter(cross_1_comp, cross_1_comp.begin()));

                for (auto it = crossed_1.begin(); it != crossed_1.end(); it++) {
                    if (count(crossed_1.begin(), crossed_1.end(), *it) > 1) {
                        int index = rand() % cross_1_comp.size();
                        crossed_1[it - crossed_1.begin()] = cross_1_comp[index];
                        cross_1_comp.erase(cross_1_comp.begin() + index);
                    }
                }

                set<char> cross_2_set;
                for (auto it = crossed_2.begin(); it != crossed_2.end(); it++) {
                    cross_2_set.insert(*it);
                }
                
                vector<char> cross_2_comp;
                set_difference(alpha_set.begin(), alpha_set.end(), cross_2_set.begin(), cross_2_set.end(), inserter(cross_2_comp, cross_2_comp.begin()));

                for (auto it = crossed_2.begin(); it != crossed_2.end(); it++) {
                    if (count(crossed_2.begin(), crossed_2.end(), *it) > 1) {
                        int index = rand() % cross_2_comp.size();
                        crossed_2[it - crossed_2.begin()] = cross_2_comp[index];
                        cross_2_comp.erase(cross_2_comp.begin() + index);
                    }
                }

                cout << "1 " << crossed_1 << endl;
                cout << "2 " << crossed_2 << endl;
                

                // remove parents from inter_pop
                inter_pop.erase(inter_pop.begin());
                inter_pop.erase(inter_pop.begin() + parent_B_index);

                Chromosome chromo_A;
                chromo_A.set_key(char_set, crossed_1);
                chromo_A.set_text(text);
                chromo_A.set_fitness(freq);
                //chromo_A.fitness = ((.95 - .05) * ((chromo_A.fitness - min_fit) / (max_fit - min_fit))) + 0.05;

                Chromosome chromo_B;
                chromo_B.set_key(char_set, crossed_2);
                chromo_B.set_text(text);
                chromo_B.set_fitness(freq);
                //chromo_B.fitness = ((.95 - .05) * ((chromo_B.fitness - min_fit) / (max_fit - min_fit))) + 0.05;

                //cout << chromo_A.fitness << " "; chromo_A.print_key(); cout << endl;
                //cout << chromo_B.fitness << " "; chromo_B.print_key(); cout << endl;

                // add crossed chromosomes to next gen
                new_pop.push_back(chromo_A);
                new_pop.push_back(chromo_B);
            }
        }

        //cout << "EXITED" << endl;
        //exit(EXIT_SUCCESS);

        
        //for (int i = 0; i < pop_size; i++) {
        //    cout << new_pop[i].fitness << " ";
        //}
        //cout << endl << endl;;

        // MUTATION
        for (auto it = new_pop.begin(); it != new_pop.end(); it++) {
            if (((double) rand() / (RAND_MAX)) <= 0.05) {
                
            }
        }


        population.clear();
		copy(new_pop.begin(), new_pop.end(), back_inserter(population));
		gen++;
    }


    // create pop
    //      gen 100 random keys A -> (int) or (char)

    // create inter pop

    cout << "EXIT SUCCESS" << endl;
}