// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

namespace Borek {

class UUID {
public:
        UUID();
        explicit UUID(uint64_t value);
        operator uint64_t();
        uint64_t GetValue();

private:
        uint64_t m_Value;
};

}  // namespace Borek
