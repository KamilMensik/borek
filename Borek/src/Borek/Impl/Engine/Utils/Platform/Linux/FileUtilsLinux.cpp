// Copyright 2024-2025 <kamilekmensik@gmail.com>

#ifndef BR_PLATFORM_WINDOWS

#include <string>
#include <unistd.h>
#include <pwd.h>

namespace Borek {
namespace Utils {

std::string UserHomePath()
{
        return getpwuid(getuid())->pw_dir;
}

}  // namespace Utils
}  // namespace Borek

#endif
