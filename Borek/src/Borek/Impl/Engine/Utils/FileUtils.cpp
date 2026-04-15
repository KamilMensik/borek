// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <string>

#include "Include/Engine/Utils/FileUtils.h"

namespace Borek {
namespace Utils {

std::string UserDataPath()
{
        return UserHomePath().append("/.Borek");
}

}  // namespace Utils
}  // namespace Borek
