// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
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

        return fs::relative(path, Application::ProjectPath());
}

fs::path FromRelative(const fs::path path)
{
        using Utils::Settings;

        return fs::path(Application::ProjectPath()) / path;
}

}  // namespace Path
}  // namespace Utils
}  // namespace Borek
