#include <string>

struct Item
{
  std::string id;
  int weight = 0;
  int value = 0;
  double ratio = 0;
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
