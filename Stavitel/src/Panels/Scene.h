// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Node.h"
#include <Borek/Include/Base/Entity.h>
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Scene {
public:
        Scene();
        void OnImguiRender();
        void SendOnSelectionChangeEvent();
        inline Entity GetSelectedEntity() { return m_SelectedEntity->entity; }

private:
        Node* m_SelectedEntity;
        Node* m_RootNode;

        void DrawTreeNode(Node* node);
};

}  // namespace Panels
}  // namespace Borek
