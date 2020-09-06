#include "RNG.h"

RNG::RNG()
{
    std::random_device rd;
    generator = std::mt19937(rd());
}

RNG::RNG(std::mt19937 _generator)
    : generator(_generator) {}

int RNG::random_number(int max)
{
    if (max == 0)
    {
        return 0;
    }
    std::uniform_int_distribution<> distribution(1, max);
    return distribution(generator);
}
