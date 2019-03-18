#include <iostream>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <random>
#include <time.h>


struct Chromosome {
    
    std::map<char, char> key;
    //std::vector<std::pair<std::string,int>> occurences;

    std::string text;
    double fitness;
    
    void set_key(std::set<char> char_set, std::string str);
    void set_text(std::string str);
    void set_fitness(std::vector<std::map<std::string,double>> freq);

    void print_key() const;

    friend bool operator< (const Chromosome& lhs, const Chromosome& rhs);



   
};

void Chromosome::set_key(std::set<char> char_set, std::string str) {
    key.clear();
    for (int i = 0; i < str.length(); i++) {
        auto it = char_set.begin();
        advance(it, i);
        key[*it] = (char) str[i];
    }
}

void Chromosome::set_text(std::string str) {
    std::string str_1 = "";
    for (int i = 0; i < str.size(); i++) {
        str_1 += key[str[i]];
    }
    text = str_1;
}

void Chromosome::set_fitness(std::vector<std::map<std::string,double>> freq) {
    // UNI
    // if 5 most common chars are in top ten of freq + 1
    // in top 15 + .5
    // in top 20 + .25
    
    // count occurances of char
    int i = 0;
    double uni_fit = 0;
    for (auto it = freq[0].begin(); it != freq[0].end(), i < 10; it++) {
        
        // find num of times occur
        int k = 0;
        size_t pos = text.find(it->first, 0);
        while (pos != std::string::npos) {
            k++;
            pos = text.find(it->first, pos + 1);
        }
        uni_fit += k;
        i++;
    }
    //fitness = uni_fit;

    /*i = 0;
    double di_fit = 0;
    for (auto it = freq[1].begin(); it != freq[1].end(), i < 10; it++) {
        
        // find num of times occur
        int k = 0;
        size_t pos = text.find(it->first, 0);
        while (pos != std::string::npos) {
            k++;
            pos = text.find(it->first, pos + 1);
        }
        di_fit += k;
        i++;
    }*/


    i = 0;
    double tri_fit = 0;
    for (auto it = freq[2].begin(); it != freq[2].end(), i < 5; it++) {
        
        // find num of times occur
        int k = 0;
        size_t pos = text.find(it->first, 0);
        while (pos != std::string::npos) {
            k++;
            pos = text.find(it->first, pos + 1);
        }
        tri_fit += k;
        i++;
    }

    //std::cout  << "ASDASD";
    fitness = uni_fit + (3 * tri_fit);
    //fitness = uni_fit;



    //fitness = uni_fit + (di_fit * 2) + (tri_fit * 3);
    





    // BI

    // TRI
}



 void Chromosome::print_key() const {
        for_each(key.begin(), key.end(), [](auto it) { std::cout << std::setw(3) << it.first << " "; });
        std::cout << std::endl;
        for_each(key.begin(), key.end(), [](auto it) { std::cout << std::setw(3) << it.second << " "; });
        std::cout << std::endl;
        //for_each(key.begin(), key.end(), [](auto it) { std::cout << std::setw(3) << it.second.second << " "; });
        
}

bool operator< (const Chromosome& lhs, const Chromosome& rhs) {
    return (lhs.fitness < rhs.fitness);
}
