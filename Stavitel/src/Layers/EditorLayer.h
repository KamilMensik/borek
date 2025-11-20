// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Core.h"
#include "Panels/ConsolePanel.h"
#include "Panels/Import.h"
#include "Panels/Test.h"
#include "Panels/TilesetPanel.h"
#include "Panels/ToolbarPanel.h"
#include <glm/ext/vector_float2.hpp>

#include <Borek/Include/Base/Layer.h>

#include "Panels/GizmoPanel.h"
#include "Panels/Properties.h"
#include "Panels/Scene.h"
#include "Panels/AssetsPanel.h"


namespace Borek {

class EditorLayer : public Layer {
public:
        EditorLayer();
        ~EditorLayer() override;

        void OnImGuiRender() override;
        void BeginDockspace();
        void EndDockspace();

        void OnUpdate(float delta) override;
        void OnGameStarted();
        void OnGameEnded();
        void SetSelectedEntity(Entity e);

        Panels::ConsolePanel& GetConsole();

        const glm::vec2& GetViewportSize();
        const glm::vec2& GetViewportPosition();

private:
        Panels::Properties m_PropertiesPanel;
        Panels::Scene m_ScenePanel;
        Panels::GizmoPanel m_GizmoPanel;
        Panels::Assets m_AssetsPanel;
        Panels::Toolbar m_ToolbarPanel;
        Panels::Test m_TestPanel;
        Panels::ConsolePanel m_Console;
        Panels::Import m_ImportPanel;
        Panels::Tileset m_TilesetPanel;

        glm::vec2 m_ViewportSize = glm::vec2(1280, 720);
        glm::vec2 m_ViewportPosition;
        Time m_OldTime;
        bool m_IsFocused;
};

}  // namespace Borek
