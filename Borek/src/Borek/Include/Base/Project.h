// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Utils/Changeable.h"
#include <filesystem>

namespace Borek {

struct Project {
        std::filesystem::path last_scene_opened_path;
        std::string start_scene_path;

        static void
        Serialize(const std::string& path);

        static void
        Deserialize(const std::string& path);

        void
        OnChange();

        static const Project&
        Instance()
        {
                return s_Instance;
        }

        Changeable<Project>
        static InstanceM()
        {
                return Changeable<Project>(s_Instance);
        }

private:
        static Project s_Instance;
};

}  // namespace Borek
