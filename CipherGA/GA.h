#include <random>
#include <vector>
#include <utility>
#include <string>

class GACipher
{
  private:
    // Adjustable Knobs for the Genetic Algorithm
    double (*fitnessFunc)(std::string cipher);
    double copyRate;
    double mutationRate;
    int popSize;
    std::string cipher;
    std::vector<std::pair<std::string,double>> population;

    // Random Engine
    std::mt19937 engine{static_cast<long unsigned int>(time(0))};
    std::uniform_real_distribution<double> dist{0.0, 1.0};

  public:
    // Constructors
    GACipher();
    GACipher(double cR, double mR, double (*fitnessFunc)(std::string cipher));
    ~GACipher();

    // Getters and Setters
    double getCopyRate() { return copyRate; }
    void setCopyRate(double cR) { copyRate = cR; }

    double getMutationRate() { return mutationRate; }
    void setMutationRate(double mR) { mutationRate = mR; }

    double getPopSize() { return popSize; }
    void setPopSize(double pS) { popSize = pS; }

    // Methods
    void run();
    void loadFreq(std::string fileName);
    void loadCipher(std::string fileName);
    void randPopulation();
};

GACipher::GACipher()
{

}

GACipher::GACipher(double cR, double mR, double (*fitnessFunc)(std::string cipher))
{

}

GACipher::~GACipher()
{

}

void GACipher::randPopulation()
{
  std::vector<char> alphabet = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
  std::vector<char> genGeneList;
  int randIndex;
  std::pair<std::string, double> temp;
  while (population.size() <= popSize)
  {
    genGeneList = alphabet;
    temp.first = "";
    for (int i = 0; i < 26; i++)
    {
      randIndex = (dist(engine)*genGeneList.size());
      temp.first += genGeneList[randIndex];
      genGeneList.erase(genGeneList.begin()+randIndex);
    }
    population.push_back(temp);
  }
}

void GACipher::loadFreq(std::string fileName)
{

}

void GACipher::loadCipher(std::string fileName)
{

}

void GACipher::run()
{

}
