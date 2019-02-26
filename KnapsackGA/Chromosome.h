#include "Item.h"
#include <vector>

struct Chromosome {
    std::vector<bool> gene;
    int value = 0;
    int weight = 0;
    int fitness = 0;
    // do we need a ratio??

    Chromosome(std::vector<Item> items, std::vector<bool> chromo);
    friend std::ostream& operator<<(std::ostream& os, const Chromosome& chromo);
    friend bool operator< (const Chromosome& lhs, const Chromosome& rhs);
};

Chromosome::Chromosome(std::vector<Item> items, std::vector<bool> chromo) {
    gene = chromo;
    for (auto it = chromo.begin(); it != chromo.end(); it++) {
        if (*it) {
            weight += items.at(it - chromo.begin()).weight;
            value += items.at(it - chromo.begin()).value;
        }
    }

    // FITNESS FUNCTION DEFINED HERE //
    fitness = value;
}

std::ostream& operator<<(std::ostream& os, const Chromosome& chromo) {
    for (auto it = chromo.gene.begin(); it != chromo.gene.end(); it++)
        os << *it << " ";
    // os << "\tW: " << chromo.weight << "\tV: " << chromo.value << std::endl;
    os << "\tF: " << chromo.fitness << std::endl;
    return os;
}

bool operator< (const Chromosome& lhs, const Chromosome& rhs) {
    return (lhs.fitness < rhs.fitness);
}