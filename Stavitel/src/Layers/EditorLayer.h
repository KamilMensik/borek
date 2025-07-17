// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/ApplicationEvents.h"
#include "Panels/ReplPanel.h"
#include "Panels/ToolbarPanel.h"
#include <glm/ext/vector_float2.hpp>

#include <Borek/Include/Events/KeyEvents.h>
#include <Borek/Include/Events/MouseEvents.h>
#include <Borek/Include/Base/Layer.h>

#include "Events/Events.h"
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

        void OnEvent(Event &e) override;
        void OnGameStarted();
        void OnGameEnded();

private:
        Panels::Properties m_PropertiesPanel;
        Panels::Scene m_ScenePanel;
        Panels::GizmoPanel m_GizmoPanel;
        Panels::Assets m_AssetsPanel;
        Panels::Toolbar m_ToolbarPanel;
        Panels::Repl m_Repl;

        glm::vec2 m_ViewportSize = glm::vec2(1280, 720);
        bool m_IsFocused;

private:
        bool OnScenePanelSelectedEvent(ScenePanelSelectedEvent& ev);
        bool OnSceneChangedEvent(SceneChangedEvent& ev);
};

}  // namespace Borek
