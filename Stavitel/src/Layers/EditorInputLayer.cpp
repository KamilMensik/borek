// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Layers/EditorInputLayer.h"
#include "Include/Base/Application.h"
#include "Include/Events/KeyEvents.h"
#include "Misc/Notifications/Notifications.h"
#include "Misc/SceneTabBar.h"

namespace Borek {

static bool
on_ctrl(KeyEvent& e)
{
        if (Application::IsPlaying())
                return false;

        if (e.IsPressed() && !e.IsRepeated()) {
                switch (e.GetKeycode()) {
                case KeyCode::Z:
                        SceneTabBar::GetCommandHandler().Undo();
                        Notifications::Add(Notification()
                                .Text("Undoed")
                                .Type(NotificationType_Error));
                        break;
                case KeyCode::Y:
                        SceneTabBar::GetCommandHandler().Redo();
                        Notifications::Add(Notification()
                                .Text("Redoed")
                                .Type(NotificationType_Info));
                        break;
                case KeyCode::S:
                        SceneTabBar::SaveScene();
                        Notifications::Add(Notification()
                                .Text("Scene Successfully Saved")
                                .Type(NotificationType_Success));
                        break;
                default:
                        return false;
                }
        }

        return true;
}

static bool
on_key(KeyEvent& e)
{
        if (Input::IsKeyPressed(KeyCode::LEFT_CONTROL))
                return on_ctrl(e);

        return false;
}

void EditorInputLayer::OnAttach()
{
        m_EditorCamera.width = 1280;
        m_EditorCamera.height = 720;

        m_EventHandles[0] = MouseScrolledEvent::AddListener(EVENT_FN(OnMouseScrolled));
        m_EventHandles[1] = MouseButtonEvent::AddListener(EVENT_FN(OnMouseButton));
        m_EventHandles[2] = MouseMovedEvent::AddListener(EVENT_FN(OnMouseMoved));
        m_EventHandles[3] = KeyEvent::AddListener(on_key);
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

CameraComponent&
EditorInputLayer::GetCamera()
{
        return m_EditorCamera;
}

bool
EditorInputLayer::OnMouseScrolled(MouseScrolledEvent& ev)
{
        if (Application::IsPlaying())
                return false;

        m_EditorCamera.zoom = std::max(
                m_EditorCamera.zoom - ev.GetAmountY() * m_EditorCamera.zoom / 10,
                0.01f);

        return true;
}

bool
EditorInputLayer::OnMouseButton(MouseButtonEvent& e)
{
        if (e.IsPressed()) {
                glm::vec2 mouse_pos_r = Input::GetMousePosRelative();
                if (std::abs(mouse_pos_r.x) > 1.0f ||
                        std::abs(mouse_pos_r.y) > 1.0f)
                        return false;

                if (e.GetButton() == MouseButton::BUTTON_RIGHT) {
                        m_IsDragging = true;
                        m_MouseOffset = m_CameraTransform.position;
                }
        } else {
                if (e.GetButton() == MouseButton::BUTTON_RIGHT) {
                        m_IsDragging = false;
                }
        }

        return true;
}

bool
EditorInputLayer::OnMouseMoved(MouseMovedEvent& ev)
{
        if (!Application::IsPlaying() && m_IsDragging) {
                glm::vec2 new_pos = (ev.GetPos() - m_MouseS) * m_EditorCamera.zoom * 2.0f;
                new_pos.x *= -1;
                m_CameraTransform.position = new_pos + m_MouseOffset;
        } else {
                m_MouseS = ev.GetPos();
        }

        return true;
}

}  // namespace Borek
