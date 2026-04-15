// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <random>
namespace Borek {

class Random
{
public:
        static void
        Init();

        static float
        Float();

        static float
        Float(float to);

        static float
        Float(float from, float to);

        static uint32_t
        Int();

        static uint32_t
        Int(uint32_t to);

        static uint32_t
        Int(uint32_t from, uint32_t to);

private:
        static std::mt19937 s_Generator;
        static std::uniform_real_distribution<> s_FloatDistribution;
        static std::uniform_int_distribution<std::mt19937::result_type> s_IntDistribution;
};

}  // namespace Borek
