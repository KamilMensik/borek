// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

namespace Borek {

class SceneTreeExplorer{
public:
        SceneTreeExplorer() = default;

        SceneTreeExplorer&
        Search(std::string_view value);

        SceneTreeExplorer&
        OnClick(std::function<void(Entity)> callback);

        SceneTreeExplorer&
        OnRightClick(std::function<void(Entity)> callback);

        SceneTreeExplorer&
        Selected(Entity e);

        SceneTreeExplorer&
        AllowReorder();

        SceneTreeExplorer&
        ContextMenu(std::function<void(Entity)> callback);

        void
        Draw(float y_percent = 1.0f);

private:
        std::string_view m_Search;
        Entity m_SelectedEntity;
        Entity m_Skip;
        std::vector<std::pair<int, Entity>> m_FlatGraph;
        Set<uint32_t> m_VisibleTreeNodes;
        std::function<void(Entity)> m_OnClickCallback = [](Entity){};
        std::function<void(Entity)> m_OnRightClickCallback = [](Entity){};
        std::function<void(Entity)> m_ContextMenu = [](Entity){};
        bool m_AllowReorder = false;
        bool m_AllowContextMenu = false;

        void
        Phase1IterFuncBegin(Entity e);

        void
        Phase1IterFuncEnd(Entity e);

        void
        Phase2();

        void
        DrawTree();
};

}  // namespace Borek
