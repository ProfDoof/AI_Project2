#include <cmath>
#include <iostream>
using namespace std;

struct snowglobe
{
  string id;
  int weight;
  int value;
  double ratio;
};

bool sortWeight(snowglobe a, snowglobe b)
{
  return a.weight < b.weight;
}

bool sortValue(snowglobe a, snowglobe b)
{
  return a.value > b.value;
}

bool sortRatio(snowglobe a, snowglobe b)
{
  return a.ratio > b.ratio;
}
