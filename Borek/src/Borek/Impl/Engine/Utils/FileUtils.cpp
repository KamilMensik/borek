// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <string>

#include <Include/Engine/Utils/tinyfiledialogs.h>

#include "Include/Engine/Utils/FileUtils.h"

namespace Borek {
namespace Utils {

std::string OpenFileDialog(const char* filter, const char* default_path)
{
        return tinyfd_openFileDialog("Open file", default_path, 0, NULL, "", 0);
}

std::string SaveFileDialog(const char* filter, const char* default_path)
{
        return tinyfd_saveFileDialog("Open file", default_path, 0, NULL, "");
}

std::string OpenFolderDialog(const char* default_path)
{
        return tinyfd_selectFolderDialog("Select folder", default_path);
}

std::string UserDataPath()
{
        return UserHomePath().append("/.Borek");
}

}  // namespace Utils
}  // namespace Borek
