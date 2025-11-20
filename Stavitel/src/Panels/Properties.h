// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include "Include/Events/Event.h"
#include "Panels/PanelEvents.h"
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Properties {
public:
        Properties();
        ~Properties();
        void OnImguiRender();
        void OnChangeEntity(ChangeEntityEvent& e);

private:
        Borek::Entity m_Entity;
        std::array<EVHandle, 2> m_EventHandles;
};

}  // namespace Panels
}  // namespace Borek
