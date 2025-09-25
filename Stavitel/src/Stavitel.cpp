// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Query.h"
#include "Include/Base/Renderer2D.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/FZX/Body.h"
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <Borek.h>
#include "EditorState.h"
#include "Include/Base/Colors.h"
#include "Include/Base/Components.h"
#include "Include/Base/Input.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/AssetManager.h"
#include "Include/Engine/EntityInitializer.h"
#include "Include/Events/ApplicationEvents.h"
#include "Include/Events/EventCaller.h"
#include "Include/Events/MouseEvents.h"
#include "glm/ext/vector_float2.hpp"
#include "Borek/Include/Engine/SceneSerializer.h"
#include "Borek/Include/Engine/Utils/Settings.h"
#include "Borek/Include/Engine/Utils/GeometryUtils.h"

#include "Layers/EditorLayer.h"
#include "EditorSettings.h"

namespace Borek {

static Ref<Graphics::FrameBuffer> fb;

class Stavitel : public Application
{
public:
        Stavitel() : Borek::Application("Borek Stavitel!")
        {
                m_EditorLayer = new EditorLayer();
                PushLayer(m_EditorLayer);
                s_LogFunc = [this](const std::string& str) {
                        std::stringstream ss;
                        ss << str << "\033[39m\033[49m";
                        std::string res = ss.str();
                        glm::vec4 color;
                        switch (str[3]) {
                        case '1':
                                color = Colors::RED;
                                break;
                        case '2':
                                color = Colors::GREEN;
                                break;
                        case '3':
                                color = Colors::YELLOW;
                                break;
                        case '4':
                                color = Colors::CYAN;
                                break;
                        case '6':
                                color = Colors::BLUE;
                                break;
                        }
                        m_EditorLayer->GetConsole().Add(res.substr(10, res.size() - 20), color);
                        std::cout << res << "\n";
                };

                Graphics::FrameBufferSettings fb_settings;
                fb_settings.width = 1280;
                fb_settings.height = 720;
                fb = Graphics::FrameBuffer::Create(fb_settings);

                AssetManager::AssetifyWorkspace(Utils::Settings::Instance().current_project_path, true);
        }

        ~Stavitel()
        {
        }

        void OnEvent(Event& e) override
        {
                Application::OnEvent(e);

                EventCaller ec(e);
                ec.TryCall<MouseMovedEvent>(EVENT_FN(Stavitel::OnMouseMovedEvent));
                ec.TryCall<MouseButtonPressedEvent>(EVENT_FN(Stavitel::OnMouseButtonPressed));
                ec.TryCall<MouseButtonReleasedEvent>(EVENT_FN(Stavitel::OnMouseButtonReleased));
                ec.TryCall<AddComponentEvent>(EVENT_FN(Stavitel::OnAddComponentEvent));
                ec.TryCall<RemoveComponentEvent>(EVENT_FN(Stavitel::OnRemoveComponentEvent));
        }

        void OnUpdate(float delta) override
        {
                static unsigned tick_counter = 30;

                if (IsPlaying() && !m_Started) {
                        auto path = Utils::Settings::Instance().last_scene_opened_path;
                        m_EditorLayer->OnGameStarted();
                        SceneSerializer(Application::GetScene()).Serialize(path);
                        m_Started = true;
                        m_CurrentScene->TraverseScene(EntityInitializer::InitializeBegin,
                                                      EntityInitializer::InitializeEnd);
                } else if (!IsPlaying() && m_Started) {
                        m_EditorLayer->OnGameEnded();
                        auto path = Utils::Settings::Instance().last_scene_opened_path;
                        SetScene(NewRef<Scene>());
                        SceneSerializer(m_CurrentScene).Deserialize(path);
                        m_Started = false;
                }

                if (--tick_counter <= 0) {
                        AssetManager::AssetifyWorkspace(Utils::Settings::Instance().current_project_path);
                        tick_counter = 30;
                }

                Application::OnUpdate(delta);

                if (EditorSettings::show_collision_shapes) {
                        for (auto [id, body] : Query<IDComponent, FZX::BodyComponent>()) {
                                Entity e(id->ecs_id);
                                Renderer2D::DrawQuadOutline(e.GlobalTransform(), Colors::BLUE);
                        }
                }
        }

        virtual void OnImGuiRenderBegin() override
        {
                Application::OnImGuiRenderBegin();
                m_EditorLayer->BeginDockspace();
        }

        virtual void OnImguiRenderEnd() override
        {
                Application::OnImguiRenderEnd();
                m_EditorLayer->EndDockspace();
        }

        virtual std::pair<glm::vec2, glm::vec2>
        GetMouseOffsetInternal() override
        {
                return { m_EditorLayer->GetViewportPosition(),
                         m_EditorLayer->GetViewportSize() };
        }

        bool OnMouseButtonPressed(MouseButtonPressedEvent& ev)
        {
                glm::vec2 mouse_pos_r = Input::GetMousePosRelative();
                if (std::abs(mouse_pos_r.x) > 1.0f ||
                        std::abs(mouse_pos_r.y) > 1.0f)
                        return false;

                if (ev.GetButton() == MouseButton::BUTTON_RIGHT) {
                        m_IsDragging = true;
                        m_MouseOffset = m_EditorCameraTransform.position;
                }

                if (ev.GetButton() == MouseButton::BUTTON_LEFT) {

                        FZX::SmallList<uint32_t> res;
                        glm::vec2 pos = Utils::Geometry::rotate_point(
                                glm::vec2(0), mouse_pos_r * glm::vec2(1280 / 4, 720 / 4),
                                m_CameraTransform.rotation
                        );

                        pos += m_CameraTransform.position;
                        m_SpriteGrid.GetCollisions(pos, UINT32_MAX, &res);

                        uint32_t last_id = UINT32_MAX;
                        for (uint32_t val : res) {
                                last_id = val;
                        }

                        m_EditorLayer->SetSelectedEntity(Entity(last_id));
                }

                return false;
        }

        bool OnMouseButtonReleased(MouseButtonReleasedEvent& ev)
        {
                if (ev.GetButton() == MouseButton::BUTTON_RIGHT) {
                        m_IsDragging = false;
                }

                return false;
        }

        bool OnMouseMovedEvent(MouseMovedEvent& ev)
        {
                if (!IsPlaying() && m_IsDragging) {
                        glm::vec2 new_pos = ev.GetPos() - m_MouseS;
                        new_pos.x *= -1;
                        m_EditorCameraTransform.position = new_pos + m_MouseOffset;
                } else {
                        m_MouseS = ev.GetPos();
                }

                return false;
        }

        bool OnAddComponentEvent(AddComponentEvent& ev)
        {
                Entity(ev.GetEntityId()).AddComponent(ev.GetId());
                return true;
        }

        bool OnRemoveComponentEvent(RemoveComponentEvent& ev)
        {
                Entity(ev.GetEntityId()).RemoveComponent(ev.GetId());
                return true;
        }

        void SetCamera() override
        {
                if (IsPlaying()) {
                        Application::SetCamera();
                } else {
                        m_Camera = &m_EditorCamera;
                        m_CameraTransform = m_EditorCameraTransform;
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

        bool m_IsDragging;
        glm::vec2 m_MouseOffset;
        glm::vec2 m_MouseS;
};

}  // namespace Borek

BOREK_APPLICATION(Borek::Stavitel)
