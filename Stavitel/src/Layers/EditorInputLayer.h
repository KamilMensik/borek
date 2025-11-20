// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Layer.h"
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

private:
        TransformComponent m_CameraTransform;
        glm::vec2 m_MouseOffset;
        glm::vec2 m_MouseS;
        std::array<EVHandle, 3> m_EventHandles;
        bool m_IsDragging = false;

        void
        OnMouseMoved(MouseMovedEvent& e);

        void
        OnMouseButton(MouseButtonEvent& e);

        void
        OnMouseScrolled(MouseScrolledEvent& e);
};

}  // namespace Borek
