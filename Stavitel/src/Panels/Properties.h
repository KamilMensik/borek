// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Properties {
public:
        Properties();
        void OnImguiRender();
        void ChangeEntity(Entity e);

private:
        Borek::Entity m_Entity;
};

}  // namespace Panels
}  // namespace Borek
