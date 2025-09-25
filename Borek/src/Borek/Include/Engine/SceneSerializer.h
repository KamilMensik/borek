// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

#include "Include/Base/Scene.h"
#include "Include/Core.h"

namespace Borek {

class SceneSerializer {
public:
        SceneSerializer(Ref<Scene> scene);
        void Serialize(const std::string& path);
        void Deserialize(const std::string& path);

private:
        Ref<Scene> m_Scene;
};

}  // namespace Borek
