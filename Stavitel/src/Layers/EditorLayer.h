// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/MouseEvents.h"
#include <glm/ext/vector_float2.hpp>

#include "Events/Events.h"
#include "Include/Base/Entity.h"
#include "Include/Base/Query.h"
#include "Include/Events/KeyEvents.h"
#include "Panels/GizmoPanel.h"
#include "Panels/Properties.h"

#include "Include/Base/Layer.h"
#include "Include/Base/Scene.h"
#include "Include/Graphics/FrameBuffer.h"
#include "Include/Objects/SpriteSheet.h"
#include "Include/Objects/Sprite.h"
#include "Include/Base/Components.h"

#include "Panels/Scene.h"

namespace Borek {

class EditorLayer : public Layer {
public:
        EditorLayer(Ref<Graphics::FrameBuffer> framebuffer);
        ~EditorLayer() override;
        void OnUpdate(float delta) override;
        void OnImGuiRender() override;
        void OnEvent(Event &e) override;
        bool IsPlaying();
        Entity GetGameCamera();
        bool OnScenePanelSelectedEvent(ScenePanelSelectedEvent& ev);
        bool OnKeyPressedEvent(KeyPressedEvent& ev);
        bool OnMouseMovedEvent(MouseMovedEvent& ev);
        void BeginDockspace();
        void EndDockspace();
        void SetGameCamera();

private:
        Query m_DrawQuery;
        Query m_CameraQuery;
        Query m_ScriptableObjectQuery;

        Ref<Sprite> m_UnknownPlumber;
        Ref<SpriteSheet> m_SpriteSheet;
        Ref<SubSprite> m_Subsprite;
        Ref<Scene> m_Scene;
        Ref<Graphics::FrameBuffer> m_FrameBuffer;

        Panels::Properties m_PropertiesPanel;
        Panels::Scene m_ScenePanel;
        Panels::GizmoPanel m_GizmoPanel;
        glm::vec2 m_ViewportSize = glm::vec2(1280, 720);
        bool m_IsFocused;

        CameraComponent m_EditorCamera;
        TransformComponent m_EditorCameraTransform;

        void LoadScene(const std::string& path);
};

}  // namespace Borek
