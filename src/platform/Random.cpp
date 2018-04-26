//
// Created by A Hubbers on 21/04/2018.
//

#include <cstdlib>
#include <ctime>
#include "Random.h"

Random::Random() {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

}

int Random::get(int min, int max) {
  return min + (std::rand() % static_cast<int>(max - min + 1));
}
