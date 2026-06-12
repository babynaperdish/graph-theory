#include "RandomGenerator.h"

RandomGenerator::RandomGenerator()
    : gen(std::random_device{}())
{
}

double RandomGenerator::uniform()
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

int RandomGenerator::uniformInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
