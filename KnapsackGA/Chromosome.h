#include "Item.h"
#include <vector>

struct Chromosome {
    std::vector<bool> gene;
    int value = 0;
    int weight = 0;
    double fitness = 0;

    void mutate(int index);

    Chromosome(std::vector<Item> items, std::vector<bool> chromo);
    friend std::ostream& operator<<(std::ostream& os, const Chromosome& chromo);
    friend bool operator< (const Chromosome& lhs, const Chromosome& rhs);
};


void Chromosome::mutate(int index) {
    gene.at(index) = !gene.at(index);
}

Chromosome::Chromosome(std::vector<Item> items, std::vector<bool> chromo) {
    int kWeight = 0;
    
    gene = chromo;
    for (auto it = chromo.begin(); it != chromo.end(); it++) {
        if (*it) {
            weight += items.at(it - chromo.begin()).weight;
            value += items.at(it - chromo.begin()).value;
        }
        kWeight += items.at(it - chromo.begin()).weight;
    }

    // FITNESS FUNCTION DEFINED HERE //
    fitness = value;
}

std::ostream& operator<<(std::ostream& os, const Chromosome& chromo) {
    for (auto it = chromo.gene.begin(); it != chromo.gene.end(); it++)
        os << *it << " ";
    os << chromo.fitness;
    return os;
}

bool operator< (const Chromosome& lhs, const Chromosome& rhs) {
    return (lhs.fitness < rhs.fitness);
}