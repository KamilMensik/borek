// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>

namespace Borek {
namespace Utils {
namespace Path {

std::filesystem::path ToRelative(const std::filesystem::path path);
std::filesystem::path FromRelative(const std::filesystem::path path);

}  // namespace Path
}  // namespace Utils
}  // namespace Borek
