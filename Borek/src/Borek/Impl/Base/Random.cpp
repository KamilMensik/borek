// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <random>

#include "Include/Base/Random.h"

namespace Borek {

void
Random::Init()
{
        s_Generator.seed(std::random_device()());
}

float
Random::Float()
{
        return s_FloatDistribution(s_Generator);
}

float
Random::Float(float to)
{
        return s_FloatDistribution(s_Generator) * to;
}

float
Random::Float(float from, float to)
{
        to -= from;
        return from + s_FloatDistribution(s_Generator) * to;
}

uint32_t
Random::Int()
{
        return s_IntDistribution(s_Generator);
}

uint32_t
Random::Int(uint32_t to)
{
        return s_IntDistribution(s_Generator) % to;
}

uint32_t
Random::Int(uint32_t from, uint32_t to)
{
        to -= from;
        return from + s_IntDistribution(s_Generator) % to;
}


std::mt19937 Random::s_Generator;
std::uniform_real_distribution<> Random::s_FloatDistribution(0, 1);
std::uniform_int_distribution<std::mt19937::result_type> Random::s_IntDistribution;

}  // namespace Borek
