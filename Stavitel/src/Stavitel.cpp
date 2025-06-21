// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "EditorState.h"
#include "Include/Base/Components.h"
#include "Include/Core.h"
#include "Include/Events/EventCaller.h"
#include <Borek.h>

#include "Layers/EditorLayer.h"

namespace Borek {

static Ref<Graphics::FrameBuffer> fb;

class Stavitel : public Application
{
public:
        Stavitel() : Borek::Application("Borek Stavitel!")
        {
                Graphics::FrameBufferSettings fb_settings;
                fb_settings.width = 1280;
                fb_settings.height = 720;
                fb = Graphics::FrameBuffer::Create(fb_settings);

                m_EditorLayer = new EditorLayer();
                PushLayer(m_EditorLayer);
        }

        ~Stavitel()
        {
        }

        void OnEvent(Event& e) override
        {
                Application::OnEvent(e);

                EventCaller ec(e);
                ec.TryCall<MouseMovedEvent>(EVENT_FN(Stavitel::OnMouseMovedEvent));
        }

        void OnUpdate(float delta) override
        {
                Application::OnUpdate(delta);
                if (IsPlaying() && !m_Started) {
                        OnStart();
                        m_Started = true;
                } else if (!IsPlaying() && m_Started) {
                        OnEnd();
                        m_Started = false;
                }
        }

        virtual void OnImGuiRenderBegin() override
        {
                Application::OnImGuiRenderBegin();
                m_EditorLayer->BeginDockspace();
        }

        virtual void OnImguiRenderEnd() override {
                Application::OnImguiRenderEnd();
                m_EditorLayer->EndDockspace();
        }


        bool OnMouseMovedEvent(MouseMovedEvent& ev) {
                if (!IsPlaying() && Input::IsMouseButtonPressed(MouseButton::BUTTON_RIGHT)) {
                        glm::vec2 delta = ev.GetDelta() / 400.0f;
                        m_EditorCameraTransform.position.x -= delta.x;
                        m_EditorCameraTransform.position.y += delta.y;
                }

                return false;
        }

        void SetCamera() override
        {
                if (IsPlaying()) {
                        Application::SetCamera();
                } else {
                        m_Camera = &m_EditorCamera;
                        m_CameraTransform = &m_EditorCameraTransform;
                }
        }

        virtual bool IsPlaying() override
        {
                return EditorState::game_state == GameState::kPlaying;
        }

        EditorLayer* m_EditorLayer;
        CameraComponent m_EditorCamera;
        TransformComponent m_EditorCameraTransform;
        bool m_Started = false;
};

}  // namespace Borek

BOREK_APPLICATION(Borek::Stavitel)
