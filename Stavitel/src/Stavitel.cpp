// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Entity.h"
#include "Include/Components/TilemapComponent.h"
#include "Include/Engine/EntityUninitializer.h"
#include "Include/Engine/ZIndexAssigner.h"
#include "Layers/EditorEventHandlerLayer.h"
#include "Layers/EditorInputLayer.h"
#include "Layers/ResourceLayer.h"
#include <iostream>
#include <sstream>

#include <Borek.h>
#include "EditorState.h"
#include "Include/Base/Colors.h"
#include "Include/Core.h"
#include "Include/Engine/EntityInitializer.h"
#include "glm/ext/vector_float2.hpp"
#include "Borek/Include/Engine/SceneSerializer.h"
#include "Borek/Include/Engine/Utils/Settings.h"
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
                m_ResourceLayer = new ResourceLayer();
                PushLayer(m_ResourceLayer);
                m_EditorEventHandlerLayer = new EditorEventHandlerLayer();
                PushLayer(m_EditorEventHandlerLayer);
                m_EditorInputLayer = new EditorInputLayer();
                PushLayer(m_EditorInputLayer);
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
        }

        ~Stavitel()
        {
                s_LogFunc = [](const std::string& str) {
                        std::stringstream ss;
                        ss << str << "\033[39m\033[49m";
                        std::string res = ss.str();
                        std::cout << res << "\n";
                };
        }

        void OnUpdate(float delta) override
        {
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



        void SetCamera() override
        {
                if (IsPlaying()) {
                        Application::SetCamera();
                } else {
                        m_Camera = &m_EditorCamera;
                        m_CameraTransform = m_EditorInputLayer->GetCameraTransform();
                }
        }

        virtual bool Playing() override
        {
                return EditorState::game_state == GameState::kPlaying;
        }

        EditorLayer* m_EditorLayer;
        ResourceLayer* m_ResourceLayer;
        EditorInputLayer* m_EditorInputLayer;
        EditorEventHandlerLayer* m_EditorEventHandlerLayer;
        CameraComponent m_EditorCamera;
        bool m_Started = false;

        bool m_IsDragging = false;
};

}  // namespace Borek

BOREK_APPLICATION(Borek::Stavitel)
