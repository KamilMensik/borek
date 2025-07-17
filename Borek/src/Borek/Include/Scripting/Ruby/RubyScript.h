// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <filesystem>

namespace Borek {

class RubyScript {
public:
        RubyScript(const std::filesystem::path& path);

private:
        uint32_t m_ClassSym;
};

}  // namespace Borek
