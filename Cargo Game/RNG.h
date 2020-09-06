#pragma once

#include <random>

class RNG
{
private:
    std::mt19937 generator;
public:
    RNG();
    RNG(std::mt19937 _generator);
    int random_number(int max);
};