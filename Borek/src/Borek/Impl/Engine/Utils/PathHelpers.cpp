// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <filesystem>

#include "Include/Engine/Utils/PathHelpers.h"
#include "Include/Engine/Utils/Settings.h"

namespace Borek {
namespace Utils {
namespace Path {

namespace fs = std::filesystem;

fs::path ToRelative(const fs::path path)
{
        using Utils::Settings;

        return fs::relative(path, Settings::Instance().current_project_path);
}

fs::path FromRelative(const fs::path path)
{
        using Utils::Settings;

        return fs::path(Settings::Instance().current_project_path) / path;
}

}  // namespace Path
}  // namespace Utils
}  // namespace Borek
