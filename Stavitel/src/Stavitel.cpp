// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include "Include/Components/FZXComponents.h"
#include "Include/Components/PrefabComponent.h"
#include "Include/Components/ZIndexComponent.h"
#include "Include/Debug/Log.h"
#include "Include/Events/MouseEvents.h"
#include "Include/Events/WindowEvents.h"
#include "Panels/PanelEvents.h"
#include <mrbcpp.h>
#include <glm/ext/vector_float2.hpp>

#include <Borek.h>
#include <Include/Core.h>
#include <Include/Base/Colors.h>
#include <Borek/Include/Engine/SceneSerializer.h>
#include <Borek/Include/Engine/Utils/Settings.h>
#include <Borek/Include/Drawing/Quad.h>
#include <Borek/Include/Base/Application.h>
#include <Borek/Include/Base/Project.h>
#include <Borek/Include/Engine/Exceptions/RubyException.h>
#include <Borek/Include/Engine/ZIndexAssigner.h>
#include <Borek/Include/Scripting/Ruby/Misc.h>
#include <Borek/Include/Scripting/Ruby/Modules/RBChangesExporter.h>
#include <Borek/Include/Base/Query.h>
#include <Borek/Include/Engine/EntityInitializer.h>
#include <Borek/Include/Engine/FZX/Body.h>
#include <Borek/Include/Components/IDComponent.h>

#include "Layers/EditorLayer.h"
#include "Layers/ProjectLayer.h"
#include "EditorSettings.h"
#include "Events/ProjectEvents.h"
#include "Layers/EditorEventHandlerLayer.h"
#include "Layers/EditorInputLayer.h"
#include "Layers/ResourceLayer.h"
#include "Misc/FontAwesome.h"
#include "Misc/Notifications/Notifications.h"
#include "Misc/SceneTabBar.h"
#include "EditorState.h"

namespace Borek {

ImFont* AddFont(std::string_view path, float size)
{
        ImGuiIO& io = ImGui::GetIO();

        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;

        ImFont* res = io.Fonts->AddFontFromFileTTF(path.data(), size);
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Font Awesome1.otf"), size, &icons_config, icons_ranges);
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Font Awesome2.otf"), size, &icons_config, icons_ranges);
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Font Awesome3.otf"), size, &icons_config, icons_ranges);
        return res;
}

static Ref<Graphics::FrameBuffer> fb;

class Stavitel : public Application
{
public:
        Stavitel() : Borek::Application("Borek Stavitel!")
        {
                SwitchProjectEvent::AddListener(EVENT_FN(SwitchProject));
                MouseButtonEvent::AddListener(EVENT_FN(OnMouseButton));
                WindowResizeEvent::AddListener(EVENT_FN(OnWindowResize));
                Graphics::FrameBufferSettings fb_settings;
                fb_settings.width = 320;
                fb_settings.height = 180;
                fb = Graphics::FrameBuffer::Create(fb_settings);
                m_EditorInputLayer = new EditorInputLayer();
                PushLayer(m_EditorInputLayer);
                m_ResourceLayer = new ResourceLayer();
                PushLayer(m_ResourceLayer);
                ImGuiIO& io = ImGui::GetIO();

                AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Bold.ttf"), 24.0f);
                io.FontDefault = AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Medium.ttf"), 24.0f);
                AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Medium.ttf"), 32.0f);
                AddFont(ASSET_PATH("assets/Fonts/JetBrainsMono-Bold.ttf"), 128.0f);
                
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

                s_LogFunc = [](const std::string& str) {
                        std::stringstream ss;
                        ss << str << "\033[39m\033[49m";
                        std::string res = ss.str();
                        std::cout << res << "\n";
                };
        }

        ~Stavitel()
        {
                SceneTabBar::Deinit();
        }

        void OnUpdate(float delta) override
        {
                if (EditorState::game_state == GameState::kRestarting) {
                        m_EditorLayer->OnGameEnded();
                        auto path = Project::Instance().last_scene_opened_path;
                        SceneTabBar::ChangeScene(path, true);
                        m_EditorLayer->OnGameStarted();
                        m_Started = true;
                        EditorState::game_state = GameState::kPlaying;
                        m_CurrentScene->Initialize();

                } else if (IsPlaying() && !m_Started) {
                        m_EditorLayer->OnGameStarted();
                        SceneTabBar::SaveScene();
                        m_Started = true;
                        m_CurrentScene->Initialize();
                } else if (!IsPlaying() && m_Started) {
                        m_EditorLayer->OnGameEnded();
                        auto path = Project::Instance().last_scene_opened_path;
                        BOREK_ENGINE_INFO("Changing to scene: {}", path.string());
                        SceneTabBar::ChangeScene(path, true);
                        m_Started = false;
                        m_SpriteGrid.Clear();
                }

                Application::OnUpdate(delta);
        }

        virtual void OnImGuiRenderBegin() override
        {
                Application::OnImGuiRenderBegin();

                if (m_EditorLayer)
                        m_EditorLayer->BeginDockspace();
        }

        virtual void OnImguiRenderEnd() override
        {
                Application::OnImguiRenderEnd();

                if (m_EditorLayer)
                        m_EditorLayer->EndDockspace();
        }

        virtual std::pair<glm::vec2, glm::vec2>
        GetMouseOffsetInternal() override
        {
                if (!m_EditorLayer)
                        return Application::GetMouseOffsetInternal();

                return { m_EditorLayer->GetViewportPosition(),
                         m_EditorLayer->GetViewportSize() };
        }

        virtual void
        BeforeRenderEnd() override
        {
                if (!EditorSettings::show_collision_shapes) {
                        return;
                }

                for (auto& [id, area] : Query<IDComponent, AreaComponent>()) {
                        auto tran = Entity(id->ecs_id).GlobalTransform();
                        Drawing::Quad::Draw(tran.position, scale_tof(tran.scale) * glm::vec2(area->rc.size_x, area->rc.size_y),
                                            Color(0, 255, 212, 80),
                                            ZIndexAssigner::GetTop());
                }

                for (auto& [id, body] : Query<IDComponent, BodyComponent>()) {
                        auto tran = Entity(id->ecs_id).GlobalTransform();
                        Drawing::Quad::Draw(tran.position, scale_tof(tran.scale) * glm::vec2(body->rc.size_x, body->rc.size_y),
                                            Color(0, 255, 255, 80),
                                            ZIndexAssigner::GetTop());
                }
        }



        void SetCamera() override
        {
                if (IsPlaying()) {
                        Application::SetCamera();
                } else {
                        m_Camera = &m_EditorInputLayer->GetCamera();
                        m_CameraTransform = m_EditorInputLayer->GetCameraTransform();
                }
        }

        virtual bool Playing() override
        {
                return EditorState::game_state == GameState::kPlaying;
        }

        bool
        SwitchProject(SwitchProjectEvent& e)
        {
                m_ProjectPath = e.GetPath();
                WITH_CHANGE(Utils::Settings::InstanceM(), {
                        Utils::Settings::InstanceM().current_project_path = m_ProjectPath;
                });
                if (m_ProjectPath == "") {
                        if (!m_ProjectLayer) {
                                m_ProjectLayer = new ProjectLayer();
                        }
                        PushLayer(m_ProjectLayer);

                        if (m_EditorLayer) {
                                m_Layers.Pop(m_EditorLayer);
                                m_Layers.Pop(m_EditorEventHandlerLayer);
                                delete m_EditorLayer;
                        }
                } else {
                        if (m_EditorLayer) {
                                m_Layers.Pop(m_EditorLayer);
                                delete m_EditorLayer;
                        }

                        m_EditorLayer = new EditorLayer();
                        if (!m_EditorEventHandlerLayer)
                                m_EditorEventHandlerLayer = new EditorEventHandlerLayer();

                        PushLayer(m_EditorLayer);
                        PushLayer(m_EditorEventHandlerLayer);
                        if (m_ProjectLayer) {
                                m_Layers.Pop(m_ProjectLayer);
                        }
                }

                return true;
        }

        void LoadProject() override
        {
                m_ProjectPath = Utils::Settings::ProjectPath();
                Project::Deserialize(std::filesystem::path(m_ProjectPath) / "project.bproj");
                SendEvent<SwitchProjectEvent>(m_ProjectPath);
        }

        void ChangeSceneFunc(const std::string& path) override
        {
                if (!IsPlaying()) {
                        SceneTabBar::ChangeScene(path);
                        Project::InstanceM()->last_scene_opened_path = path;
                } else {
                        Application::ChangeSceneFunc(path);
                        SceneTabBar::RuntimeChangeScene(m_CurrentScene);
                }
        }

        void
        OnRenderEnd() override
        {
                m_FrameBuffer->Unbind();
        }

        bool
        HandleRubyException(const RubyException&) override
        {
                mrb_state* mrb = m_RubyEngine->GetRubyVM();
                RBModules::ChangesExporter::Reset(mrb);
                m_EditorLayer->OnGameEnded();
                auto path = Project::Instance().last_scene_opened_path;
                SceneTabBar::ChangeScene(path, true);
                m_Started = false;
                EditorState::game_state = GameState::kStopped;
                BOREK_ENGINE_ERROR("{}", get_mrb_exc_text());
                Notifications::Add(Notification()
                        .Type(NotificationType_Error)
                        .Text("Error happened during execution, stopping game. "
                              "Check log for trace.")
                        .Title("Ruby Error"));
                return true;
        }

        bool
        OnMouseButton(MouseButtonEvent& e)
        {
                glm::vec2 mpos_rel = glm::abs(Input::GetMousePosRelative());
                if (mpos_rel.x > 1.0f || mpos_rel.y >= 1.0f)
                        return false;

                if (!IsPlaying() && e.IsPressed() && e.GetButton() == MouseButton::BUTTON_LEFT) {
                        FZX::SmallList<uint32_t> res;
                        glm::vec2 pos = Input::GetMouseWorldPos() - m_CameraTransform.position;
                        m_SpriteGrid.GetCollisions(pos, UINT32_MAX, &res);

                        if (res.size() == 0)
                                return false;

                        float zindex = std::numeric_limits<float>::min();
                        Entity top;
                        for (Entity e : res) {
                                float ezindex = e.GetComponent<ZIndexComponent>().value;
                                if (ezindex > zindex) {
                                        zindex = ezindex;
                                        top = e;
                                }
                        }

                        Entity parent = top.GetParent();
                        while (!parent.IsNil()) {
                                if (parent.HasComponent<PrefabComponent>())
                                        top = parent;

                                parent = parent.GetParent();
                        }

                        Application::SendEvent<ChangeEntityEvent>(top);
                }

                return true;
        }

        bool OnWindowResize(WindowResizeEvent& e)
        {
                m_EditorInputLayer->GetCamera().OnWindowResized(e);
                return true;
        }

        EditorLayer* m_EditorLayer = nullptr;
        ResourceLayer* m_ResourceLayer = nullptr;
        EditorInputLayer* m_EditorInputLayer = nullptr;
        EditorEventHandlerLayer* m_EditorEventHandlerLayer = nullptr;
        ProjectLayer* m_ProjectLayer = nullptr;

        bool m_Started = false;

        bool m_IsDragging = false;
};

}  // namespace Borek

BOREK_APPLICATION(Borek::Stavitel)
