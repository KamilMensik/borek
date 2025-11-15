// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Entity.h"
#include "Include/Base/Sound.h"
#include "Include/Components/PrefabComponent.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/ResourceAssetifier.h"
#include "Include/Engine/EntityUninitializer.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include "Include/Engine/ZIndexAssigner.h"
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <Borek.h>
#include "EditorState.h"
#include "Include/Base/Colors.h"
#include "Include/Base/Input.h"
#include "Include/Core.h"
#include "Include/Engine/EntityInitializer.h"
#include "Include/Events/ApplicationEvents.h"
#include "Include/Events/EventCaller.h"
#include "Include/Events/MouseEvents.h"
#include "glm/ext/vector_float2.hpp"
#include "Borek/Include/Engine/SceneSerializer.h"
#include "Borek/Include/Engine/Utils/Settings.h"
#include "Borek/Include/Engine/Utils/GeometryUtils.h"
#include "Borek/Include/Drawing/Quad.h"
#include "Include/Base/Query.h"
#include "Include/Base/Renderer2D.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Components/IDComponent.h"

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
                fb_settings.width = 320;
                fb_settings.height = 180;
                fb = Graphics::FrameBuffer::Create(fb_settings);

                ResourceAssetifier::AssetifyFolder(Utils::Settings::Instance().current_project_path);
        }

        ~Stavitel()
        {
        }

        void OnEvent(Event& e) override
        {
                Application::OnEvent(e);

                EventCaller ec(e);
                ec.TryCall<MouseMovedEvent>(EVENT_FN(Stavitel::OnMouseMovedEvent));
                ec.TryCall<MouseScrolledEvent>(EVENT_FN(Stavitel::OnMouseScrolled));
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
                        Ref<Scene> old_scene = m_CurrentScene;
                        old_scene->TraverseScene(EntityUninitializer::UninitializeBegin,
                                                 EntityUninitializer::UninitializeEnd);
                        SetScene(NewRef<Scene>());
                        SceneSerializer(m_CurrentScene).Deserialize(path);
                        m_Started = false;
                }

                if (--tick_counter <= 0) {
                        ResourceAssetifier::AssetifyFolder(Utils::Settings::Instance().current_project_path);
                        for (auto& [id, prefab] : Query<IDComponent, PrefabComponent>()) {
                                prefab->Update(id->ecs_id);
                        }
                        tick_counter = 30;
                }

                Application::OnUpdate(delta);

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

        virtual void
        BeforeRenderEnd() override
        {
                if (!EditorSettings::show_collision_shapes) {
                        return;
                }

                for (auto [id, body] : Query<IDComponent, FZX::BodyComponent>()) {
                        Entity e(id->ecs_id);
                        const TransformComponent tran = e.GlobalTransform();
                        Drawing::Quad::Draw(tran.position, tran.scale, Color(0, 1, 1, 0.2), ZIndexAssigner::GetTop());
                }

                for (auto [id, tc] : Query<IDComponent, TilemapComponent>()) {
                        Entity e(id->ecs_id);
                        auto global_transform = e.GlobalTransform();
                        const glm::vec2& scale = global_transform.scale;
                        const SpriteSheetAsset& spritesheet = tc->tilemap->sprite_sheet.Convert();
                        const glm::vec2 tile_size(spritesheet.step_x * scale.x,
                                                  spritesheet.step_y * scale.y);

                        for (auto it : tc->items) {
                                const auto& pos = it.first;
                                const uint16_t index = it.second;

                                if (!tc->tilemap->tile_coliders[index].Exists())
                                        continue;

                                const glm::vec2 real_pos(pos.second * tc->step_x * scale.x, pos.first * tc->step_y * scale.y);
                                Drawing::Quad::Draw(real_pos, tile_size, Color(0, 1, 1, 0.2), ZIndexAssigner::GetTop());
                        }
                }
        }

        bool OnMouseScrolled(MouseScrolledEvent& ev)
        {
                //m_EditorCamera.zoom += ev.GetAmountY() / 10;
                return false;
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
                        glm::vec2 pos = Input::GetMouseWorldPos();
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
