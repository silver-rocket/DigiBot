#include "random.h"

int Random::nextLinear(int from, int to, size_t seed)
{
    std::mt19937 random_engine;
    std::uniform_int_distribution<> dist(from, to);

    random_engine.seed(seed);

    return dist(random_engine);
}

float Random::nextGaussian(float median, float sigma, size_t seed)
{
    std::mt19937 random_engine;
    std::normal_distribution<float> dist(median, sigma);

    random_engine.seed(seed);

    return dist(random_engine);
}
