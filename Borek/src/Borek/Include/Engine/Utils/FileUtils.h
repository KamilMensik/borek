// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

namespace Borek {
namespace Utils {

std::string OpenFileDialog(const char* filter, const char* default_path);
std::string SaveFileDialog(const char* filter, const char* default_path);
std::string OpenFolderDialog(const char* default_path);
std::string UserHomePath();
std::string UserDataPath();

}  // namespace Utils
}  // namespace Borek
