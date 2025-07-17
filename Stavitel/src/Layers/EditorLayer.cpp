// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/Events.h"
#include "Include/Base/Application.h"
#include "Include/Base/Input.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/SceneSerializer.h"
#include "Include/Engine/Utils/Settings.h"
#include "Include/Events/EventCaller.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Scripting/ScriptableObject.h"
#include "Panels/ToolbarPanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imguismo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

#include <Borek.h>
#include <Borek/Include/Base/Components.h>
#include <Borek/Include/Engine/Utils/FileUtils.h>
#include <Borek/Include/Base/Entity.h>

#include "EditorLayer.h"

namespace Borek {

class PlayerController : public ScriptableObject {
public:
        void OnCreate() override
        {
                BOREK_LOG_INFO("Player has been created");
        }

        void OnUpdate(float delta) override
        {
                if (!Input::IsKeyPressed(KeyCode::ENTER)) {
                        auto& t = GetComponent<TransformComponent>();
                        t.position += Input::GetAxis() * 100.0f * delta;
                }
        }

        void OnEvent(Event& e) override
        {
                if (e.GetEventType() == MouseButtonPressedEvent::ClassEventType()) {
                        BOREK_LOG_INFO("I have been clicked");
                }
        }
};

EditorLayer::EditorLayer()
        : Borek::Layer("TestLayer")
{
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/JetBrainsMono-Bold.ttf"), 24.0f);
        io.FontDefault = io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/JetBrainsMono-Medium.ttf"), 24.0f);

        if (Utils::Settings::Instance().last_scene_opened_path != "") {
                auto new_scene = SceneSerializer().Deserialize(Utils::Settings::Instance().last_scene_opened_path);
                Application::SetScene(new_scene);
        }

}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnEvent(Event& ev)
{
        EventCaller ec(ev);
        ec.TryCall<ScenePanelSelectedEvent>(EVENT_FN(EditorLayer::OnScenePanelSelectedEvent));
        ec.TryCall<SceneChangedEvent>(EVENT_FN(EditorLayer::OnSceneChangedEvent));
}

void EditorLayer::BeginDockspace()
{
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("Dockspace", nullptr, window_flags)) {
                ImGui::PopStyleVar();
                ImGui::PopStyleVar(2);

                // Submit the DockSpace
                ImGuiIO& io = ImGui::GetIO();
                ImGuiStyle& style = ImGui::GetStyle();
                ImVec2 kept_min_size = style.WindowMinSize;
                style.WindowMinSize.x = 370;
                if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
                }
                style.WindowMinSize.x = kept_min_size.x;
        }
}
void EditorLayer::EndDockspace()
{
        ImGui::End();
}

void EditorLayer::OnImGuiRender()
{
        if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                        if (ImGui::MenuItem("Exit", NULL, false, true))
                                Application::Shutdown();
                        if (ImGui::MenuItem("Open")) {
                                auto path = Utils::OpenFileDialog("", ASSET_PATH());
                                auto new_scene = SceneSerializer().Deserialize(path);
                                Application::SetScene(new_scene);
                                WITH_CHANGE(Utils::Settings::InstanceM(), {
                                        Utils::Settings::InstanceM().last_scene_opened_path = path;
                                });
                        }
                        if (ImGui::MenuItem("Save")) {
                                auto path = Utils::Settings::Instance().last_scene_opened_path;
                                SceneSerializer(Application::GetScene()).Serialize(path);
                        }
                        if (ImGui::MenuItem("Save As")) {
                                auto path = Utils::SaveFileDialog("", ASSET_PATH());
                                SceneSerializer(Application::GetScene()).Serialize(path);
                        }
                        ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
        }

        m_ToolbarPanel.OnImguiRender(m_GizmoPanel);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Viewport")) {
                m_IsFocused = ImGui::IsWindowFocused();
                Application::GetImguiLayer().SetEventBlocking(!m_IsFocused);

                ImVec2 viewport_size = ImGui::GetContentRegionAvail();

                /* TODO: ENTITY HOVERING
                ImVec2 pos = ImGui::GetWindowPos();
                BOREK_LOG_INFO("Window pos: [{}, {}]", pos.x, pos.y);
                ImVec2 mouse_pos = ImGui::GetMousePos();
                mouse_pos.x -= pos.x;
                mouse_pos.y -= pos.y;
                mouse_pos.y = viewport_size.y - mouse_pos.y;

                BOREK_LOG_INFO("READING FROM MOUSE POS: [{}, {}]", mouse_pos.x, mouse_pos.y);

                static Query editor_query = m_Scene->Query(TransformComponent::Id());
                m_HoveredEntity = Entity();

                editor_query.each([mouse_pos, this](ECS::View& view){
                        TransformComponent& tc = view.get<TransformComponent>();
                        glm::mat2 bounds = glm::mat2(tc.position.x, tc.position.y,
                                                     tc.position.x + tc.scale.x,
                                                     tc.position.y + tc.scale.y);
                        if (bounds[0].x <= mouse_pos.x && bounds[1].x >= mouse_pos.x && bounds[0].y <= mouse_pos.y && bounds[1].y >= mouse_pos.y)
                                m_HoveredEntity = Entity(view.current_entity_id, m_Scene.get());
                });
                if (!m_HoveredEntity.IsNil()) {
                        BOREK_LOG_INFO("HOVERING OVER ENTITY: {}", m_HoveredEntity.GetComponent<TagComponent>().value.c_str());
                } else {
                        BOREK_LOG_INFO("HOVERING OVER NOTHING");
                }

                */

                uint32_t texture_id = Application::GetFramebuffer()->GetColorAttachmentId();
                ImGui::Image(texture_id, ImVec2(m_ViewportSize.x, m_ViewportSize.y),
                             ImVec2(0, 1), ImVec2(1, 0));
                if (ImGui::BeginDragDropTarget()) {
                        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_ITEM");
                        if (payload) {
                                BOREK_ENGINE_INFO("Dragged {} into viewport", std::string(SCAST<char*>(payload->Data)));
                        }
                        ImGui::EndDragDropTarget();
                }

                m_GizmoPanel.DrawGizmo();

                if (viewport_size.x > 0 && viewport_size.y > 0 &&
                    (m_ViewportSize.x != viewport_size.x ||
                     m_ViewportSize.y != viewport_size.y)) {
                        m_ViewportSize.x = viewport_size.x;
                        m_ViewportSize.y = viewport_size.y;

                        auto fb = Application::GetFramebuffer();

                        WITH_CHANGE(*fb, {
                                fb->GetSettingsM().width = m_ViewportSize.x;
                                fb->GetSettingsM().height = m_ViewportSize.y;
                        })

                        Graphics::Renderer::ResizeWindow(m_ViewportSize.x,
                                                         m_ViewportSize.y);
                }

                ImGui::End();
        }
        ImGui::PopStyleVar();

        m_ScenePanel.OnImguiRender();
        m_PropertiesPanel.OnImguiRender();
        m_AssetsPanel.OnImguiRender();
        m_Repl.OnImguiRender();
}

bool EditorLayer::OnScenePanelSelectedEvent(ScenePanelSelectedEvent& ev)
{
        m_PropertiesPanel.ChangeEntity(ev.GetEntity());
        m_GizmoPanel.ChangeEntity(ev.GetEntity());

        return false;
}

bool EditorLayer::OnSceneChangedEvent(SceneChangedEvent& ev)
{
        m_ScenePanel = Panels::Scene();
        m_PropertiesPanel.ChangeEntity(Entity());
        m_GizmoPanel.ChangeEntity(Entity());
        return false;
}


void EditorLayer::OnGameStarted()
{
        m_GizmoPanel.ChangeEntity(Entity());
        m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kNothing);
}

void EditorLayer::OnGameEnded()
{
        m_GizmoPanel.ChangeEntity(Entity());
        m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kNothing);
        m_PropertiesPanel.ChangeEntity(Entity());
        m_GizmoPanel.ChangeEntity(Entity());
}

}  // namespace Borek
