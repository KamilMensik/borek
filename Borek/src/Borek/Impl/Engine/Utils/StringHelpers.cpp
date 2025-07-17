// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cctype>
#include <string>

#include "Include/Engine/Utils/StringHelpers.h"

namespace Borek {

std::string to_snake_case(const std::string_view& str)
{
        std::string res = "";

        for (char c : str) {
                res.push_back(tolower(c));
        }

        return res;
}

}  // namespace Borek
