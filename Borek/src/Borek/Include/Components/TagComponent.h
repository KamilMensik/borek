// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/String.h"

namespace Borek {

struct TagComponent {
        String value;

        TagComponent() : value() {}
        TagComponent(const std::string& value) : value(value) {}
};

}  // namespace Borek
