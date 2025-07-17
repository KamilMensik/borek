// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <random>
#include <ctime>

#include "Include/Base/UUID.h"

namespace Borek {

static std::mt19937_64 generator(std::time(nullptr));

UUID::UUID()
{
        m_Value = generator();
}

UUID::UUID(uint64_t value) : m_Value(value) {}

UUID::operator uint64_t()
{
        return GetValue();
}

uint64_t UUID::GetValue()
{
        return m_Value;
}

}  // namespace Borek
