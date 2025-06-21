// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Panels/GizmoPanel.h"
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Toolbar {
public:
        Toolbar();
        void OnImguiRender(GizmoPanel& panel);
};

}  // namespace Panels
}  // namespace Borek
