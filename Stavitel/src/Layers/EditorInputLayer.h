// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Layer.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Events/Event.h"
#include "Include/Events/MouseEvents.h"
namespace Borek {

class EditorInputLayer : public Layer {
public:
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float delta) override;

        const TransformComponent&
        GetCameraTransform();
        
        CameraComponent&
        GetCamera();

private:
        TransformComponent m_CameraTransform;
        CameraComponent m_EditorCamera;
        glm::vec2 m_MouseOffset;
        glm::vec2 m_MouseS;
        std::array<EVHandle, 4> m_EventHandles;
        bool m_IsDragging = false;

        bool
        OnMouseMoved(MouseMovedEvent& e);

        bool
        OnMouseButton(MouseButtonEvent& e);

        bool
        OnMouseScrolled(MouseScrolledEvent& e);
};

}  // namespace Borek
