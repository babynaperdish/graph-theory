#include "LogDistribution.h"
#include <cmath>
#include <iostream>

LogDistribution::LogDistribution(double paramP, RandomGenerator& generator)
    : p(paramP), rng(generator)
{
    q = 1.0 - p;
    c = -1.0 / std::log(q);
    startP = p;
}

int LogDistribution::generate()
{
    double a = p;
    double currentP = -p / std::log(q);  // это p(1)
    double r = rng.uniform();
    int x = 1;

    while (true)
    {
        r -= currentP;
        if (r < 0)
        {
            return x;
        }
        x++;
        currentP = currentP * a * (x - 1) / x;
    }
}

double LogDistribution::getMean() const
{
    // мат ожидание
    return c * p / q;
}
