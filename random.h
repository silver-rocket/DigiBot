#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{
public:

    static int nextLinear(int from, int to, size_t seed);
    static float nextGaussian(float median, float sigma, size_t seed);
};

#endif // RANDOM_H
