#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{

public:

    static int nextLinear(int from, int to, size_t seed) {
        std::mt19937 random_engine;
        std::uniform_int_distribution<> dist(from, to);

        random_engine.seed(seed);

        return dist(random_engine);

    }

    static float nextGaussian(float median, float sigma, size_t seed) {
        std::mt19937 random_engine;
        std::normal_distribution<float> dist(median, sigma);

        random_engine.seed(seed);

        return dist(random_engine);
    }
};

#endif // RANDOM_H
