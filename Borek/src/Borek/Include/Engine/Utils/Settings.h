// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Debug/WithChange.h"
#include "Include/Debug/Assert.h"

namespace Borek {
namespace Utils {

struct Settings {
        CHANGEABLE_OBJECT(OnChange(););

        std::string current_project_path;
        std::string last_scene_opened_path;

        /*
         * Returns unmodifiable version of settings.
         */
        const static Settings& Instance()
        {
                return s_Instance;
        }

        /*
         * Returns modifiable version of settings.
         * Must be called within WITH_STATIC_CHANGE block
         */
        static Settings& InstanceM()
        {
                //BOREK_ENGINE_ASSERT(s_Instance.m_Changing, "Trying to edit settings without being in WITH_CHANGE block");
                return s_Instance;
        }

        static inline const std::string&
        ProjectPath() { return Instance().current_project_path; }

private:
        void OnChange();
        void Serialize();
        void Deserialize();
        Settings();
        static Settings s_Instance;
};

}  // namespace Utils
}  // namespace Borek
