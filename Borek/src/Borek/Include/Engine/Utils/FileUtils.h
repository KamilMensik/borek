// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

namespace Borek {
namespace Utils {

std::string OpenFileDialog(const char* filter, const char* default_path);

std::string SaveFileDialog(const char* filter, const char* default_path);

}  // namespace Utils
}  // namespace Borek
