#include <vector> 

// vector of vectors (each representing an individual in population)

struct Population {
    std::vector<std::vector<bool>> chromosomes;

    void getFittest();
    void generateChildren();
    void mutate();
}