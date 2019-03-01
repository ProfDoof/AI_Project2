#include "Item.h"
#include <vector>

struct Chromosome {
    std::vector<bool> gene;
    int value = 0;
    int weight = 0;
    double fitness = 0;

    void mutate(int index);
    void fitnessSet(int bestValue, int maxWeight);

    Chromosome(std::vector<Item> items, std::vector<bool> chromo, int maxWeight);
    Chromosome(std::vector<Item> items, std::vector<bool> chromo, int bestValue, int maxWeight);
    friend std::ostream& operator<<(std::ostream& os, const Chromosome& chromo);
    friend bool operator< (const Chromosome& lhs, const Chromosome& rhs);
};


void Chromosome::mutate(int index) {
    gene.at(index) = !gene.at(index);
}

Chromosome::Chromosome(std::vector<Item> items, std::vector<bool> chromo, int maxWeight) {
    int kWeight = 0;

    gene = chromo;
    for (int i = 0; i < gene.size(); i++) {
        if (gene[i]) {
            weight += items[i].weight;
            value += items[i].value;
        }
        kWeight += items[i].weight;
    }

    // FITNESS FUNCTION DEFINED HERE //
    fitnessSet(0, maxWeight);
}

Chromosome::Chromosome(std::vector<Item> items, std::vector<bool> chromo, int bestValue, int maxWeight) {
    // int kWeight = 0

    gene = chromo;
    for (int i = 0; i < gene.size(); i++) {
        if (gene[i]) {
            weight += items[i].weight;
            value += items[i].value;
        }
        // kWeight += items[i].weight;
    }

    // Fitness Function can probably be improved. Let's come back to this.
    // FITNESS FUNCTION DEFINED HERE //
    fitnessSet(bestValue, maxWeight);
}

void Chromosome::fitnessSet(int bestValue, int maxWeight)
{
  if (weight > maxWeight) {
    fitness = .05;
  }
  else if (bestValue = 0)
  {
    fitness = .5;
  }
  else {
    fitness = (double)(value) / (double)(bestValue);
    if (fitness < .05) {
      fitness = .055;
    }
  }
}

std::ostream& operator<<(std::ostream& os, const Chromosome& chromo) {
    for (auto it = chromo.gene.begin(); it != chromo.gene.end(); it++)
        os << *it << " ";
    os << chromo.fitness;
    return os;
}

bool operator< (const Chromosome& lhs, const Chromosome& rhs) {
    return (lhs.fitness == rhs.fitness) ? (lhs.value < rhs.value) : (lhs.fitness < rhs.fitness);
}
