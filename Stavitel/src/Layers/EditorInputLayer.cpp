// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Layers/EditorInputLayer.h"
#include "Include/Base/Application.h"

namespace Borek {

void EditorInputLayer::OnAttach()
{
        m_EventHandles[0] = MouseScrolledEvent::AddListener(EVENT_FN(OnMouseScrolled));
        m_EventHandles[1] = MouseButtonEvent::AddListener(EVENT_FN(OnMouseButton));
        m_EventHandles[2] = MouseMovedEvent::AddListener(EVENT_FN(OnMouseMoved));
}

void EditorInputLayer::OnDetach()
{
        MouseScrolledEvent::RemoveListener(m_EventHandles[0]);
        MouseButtonEvent::RemoveListener(m_EventHandles[1]);
        MouseMovedEvent::RemoveListener(m_EventHandles[2]);
}

void EditorInputLayer::OnUpdate(float delta)
{

}

const TransformComponent&
EditorInputLayer::GetCameraTransform()
{
        return m_CameraTransform;
}

void
EditorInputLayer::OnMouseScrolled(MouseScrolledEvent& ev)
{
        //m_EditorCamera.zoom += ev.GetAmountY() / 10;
}

void
EditorInputLayer::OnMouseButton(MouseButtonEvent& e)
{
        if (e.IsPressed()) {
                glm::vec2 mouse_pos_r = Input::GetMousePosRelative();
                if (std::abs(mouse_pos_r.x) > 1.0f ||
                        std::abs(mouse_pos_r.y) > 1.0f)
                        return;

                if (e.GetButton() == MouseButton::BUTTON_RIGHT) {
                        m_IsDragging = true;
                        m_MouseOffset = m_CameraTransform.position;
                }

                if (e.GetButton() == MouseButton::BUTTON_LEFT) {

                        //FZX::SmallList<uint32_t> res;
                        //glm::vec2 pos = Input::GetMouseWorldPos();
                        //m_SpriteGrid.GetCollisions(pos, UINT32_MAX, &res);

                        //uint32_t last_id = UINT32_MAX;
                        //for (uint32_t val : res) {
                        //        last_id = val;
                        //}

                        //m_EditorLayer->SetSelectedEntity(Entity(last_id));
                }
        } else {
                if (e.GetButton() == MouseButton::BUTTON_RIGHT) {
                        m_IsDragging = false;
                }
        }
}

void
EditorInputLayer::OnMouseMoved(MouseMovedEvent& ev)
{
        if (!Application::IsPlaying() && m_IsDragging) {
                glm::vec2 new_pos = ev.GetPos() - m_MouseS;
                new_pos.x *= -1;
                m_CameraTransform.position = new_pos + m_MouseOffset;
        } else {
                m_MouseS = ev.GetPos();
        }
}

}  // namespace Borek
