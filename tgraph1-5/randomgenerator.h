#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <random>

class RandomGenerator
{
private:
    std::mt19937 gen;

public:
    RandomGenerator();

    // Генерирует случайное число в [0, 1)
    double uniform();

    // Генерирует случайное целое в [min, max]
    int uniformInt(int min, int max);
};

#endif // RANDOMGENERATOR_H
