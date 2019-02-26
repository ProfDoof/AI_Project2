#include <string>

struct Item
{
  std::string id;
  int weight;
  int value;
  double ratio;
};

bool sortWeight(Item a, Item b)
{
  return a.weight < b.weight;
}

bool sortValue(Item a, Item b)
{
  return a.value > b.value;
}

bool sortRatio(Item a, Item b)
{
  return a.ratio > b.ratio;
}
