#ifndef LOGDISTRIBUTION_H
#define LOGDISTRIBUTION_H

#include "RandomGenerator.h"

class LogDistribution
{
private:
    double p;           // параметр распределения (константа)
    double q;           // = 1 - p
    double c;           // = -1 / ln(q)
    double startP;      // начальное p для алгоритма
    RandomGenerator& rng;

public:
    // Конструктор с параметром p (который будет константой)
    LogDistribution(double paramP, RandomGenerator& generator);

    // Генерирует случайное число по логарифмическому закону
    // Реализует блок-схему с рисунка 2.19
    int generate();

    // Возвращает математическое ожидание (для подбора параметра)
    double getMean() const;

    // Возвращает текущий параметр p
    double getP() const { return p; }
};

#endif // LOGDISTRIBUTION_H
