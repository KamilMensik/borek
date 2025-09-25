// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace Borek {

std::string to_snake_case(const std::string_view& str);

constexpr uint32_t
static Hash(const std::string& str)
{
        uint32_t res = 5381;

        for (auto c : str) {
                res = (res << 5) + res + c;
        }

        return res;
}

}  // namespace Borek
