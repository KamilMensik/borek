// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Commands/EntityCommands.h"
#include "EditorSettings.h"
#include "EditorState.h"
#include "Include/Base/Project.h"
#include "Misc/Notifications/Notifications.h"
#include "Misc/SceneTabBar.h"
#include "Popups/ExportPopup.h"
#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include <Borek.h>
#include <Borek/Include/Engine/Utils/FileUtils.h>
#include <Borek/Include/Base/Entity.h>
#include <Include/Base/Application.h>
#include <Include/Core.h>
#include <Include/Debug/Log.h>
#include <Include/Engine/SceneSerializer.h>
#include <Include/Graphics/Renderer.h>

#include "EditorLayer.h"
#include "Popups/EditorSettingsPopup.h"
#include "Events/ProjectEvents.h"
#include "Misc/FileExplorer/FileExplorer.h"
#include "Misc/FontAwesome.h"

namespace Borek {

static void
menu_right_size() {
        ImGuiStyle& style = ImGui::GetStyle();
        bool is_playing = EditorState::game_state == GameState::kPlaying;
        const char* pbutton_text = is_playing ? ICON_FA_ROTATE_RIGHT : ICON_FA_PLAY;
        float playbutton_width = ImGui::CalcTextSize(pbutton_text).x + style.FramePadding.x * 2.f;
        float stopbutton_width = ImGui::CalcTextSize(ICON_FA_STOP).x + style.FramePadding.x * 2.f;
        float width_needed = playbutton_width + style.ItemSpacing.x + stopbutton_width;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - width_needed);

        if (ImGui::Button(pbutton_text)) {
                if (is_playing) {
                        EditorState::game_state = GameState::kRestarting;
                } else {
                        EditorState::game_state = GameState::kPlaying;
                }
        }

        ImGui::BeginDisabled(!is_playing);
        if (ImGui::Button(ICON_FA_STOP))
                EditorState::game_state = GameState::kStopped;
        ImGui::EndDisabled();

        ImGui::EndMenuBar();
}

EditorLayer::EditorLayer()
        : Borek::Layer("TestLayer"), m_OldTime(Application::GetTime())
{
        auto& project = Project::Instance();
        SceneTabBar::ChangeScene(project.last_scene_opened_path);
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnUpdate(float delta)
{
        m_TilesetPanel.OnUpdate();
        m_ToolbarPanel.Tick();
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
        Scene& scene = *Application::GetScene();

        if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                        if (ImGui::MenuItem("Exit", NULL, false, true))
                                Application::Shutdown();
                        if (ImGui::MenuItem("Open")) {
                                //auto path = Utils::OpenFileDialog(nullptr, ASSET_PATH());
                                //SceneTabBar::ChangeScene(path);
                        }
                        if (ImGui::MenuItem("Save")) {
                                SceneTabBar::SaveScene();
                        }
                        if (ImGui::MenuItem("Save As")) {
                                //auto path = Utils::SaveFileDialog("", ASSET_PATH());
                                //SceneSerializer(Application::GetScene()).Serialize(path);
                        }
                        if (ImGui::MenuItem("Open Project")) {
                                FileExplorer::Open("Open Project",
                                                   FileExplorerType_OpenFile,
                                                   Utils::UserDataPath(),
                                                   { ".bproj", });
                        }
                        ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Editor")) {
                        if (ImGui::MenuItem("Editor Settings")) {
                                Application::OpenPopup(new Popups::EditorSettingsPopup());
                        }
                        ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Export")) {
                        Application::OpenPopup(new Popups::ExportPopup());
                        ImGui::EndMenu();
                }

                menu_right_size();
        }


        if (FileExplorer::Begin("Open Project")) {
                Application::SendEvent<SwitchProjectEvent>(std::filesystem::path(FileExplorer::GetSelected()).parent_path());
        }
        FileExplorer::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Viewport")) {
                //m_IsFocused = ImGui::IsItemHovered();
                //Application::GetImguiLayer().SetEventBlocking(!m_IsFocused);

                SceneTabBar::OnImguiRender();
                m_ToolbarPanel.OnImguiRender();

                ImVec2 viewport_size = ImGui::GetContentRegionAvail();

                ImVec2 cpos = ImGui::GetCursorScreenPos();
                m_ViewportPosition = { cpos.x, cpos.y };

                ImVec2 old_pos = ImGui::GetCursorPos();
                uint32_t texture_id = Application::GetFramebuffer()->GetColorAttachmentId();
                ImGui::Image(texture_id, ImVec2(m_ViewportSize.x, m_ViewportSize.y),
                             ImVec2(0, 1), ImVec2(1, 0));
                if (ImGui::BeginDragDropTarget()) {
                        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_EXPL_ITEM");
                        if (payload) {
                                std::string_view path(SCAST<char*>(payload->Data));
                                auto it = path.find_last_of('.');
                                if (path.substr(it) == ".scn") {
                                        glm::vec2 pos = Input::GetMouseWorldPos();
                                        glm::vec2 snap(EditorSettings::grid_snap.first,
                                                       EditorSettings::grid_snap.second);

                                        pos = glm::round(pos / snap) * snap;
                                        SceneTabBar::SendCommand<CreatePrefabCommand>(
                                                path,
                                                scene.GetTree().GetRootEntity(),
                                                pos);
                                }
                        }
                        ImGui::EndDragDropTarget();
                }
                ImVec2 cached_cursor_pos = ImGui::GetCursorPos();
                ImGui::SetCursorPos(old_pos);
                Time new_time = Application::GetTime();
                double res = 1 / (new_time.Seconds() - m_OldTime.Seconds());
                m_OldTime = new_time;
                ImGui::Text("FPS: %f", res);
                ImGui::SetCursorPos(cached_cursor_pos);

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
        m_Console.OnImguiRender();
        m_ImportPanel.OnImguiRender();
        m_TilesetPanel.OnImGuiRender();
        m_KFAnimationPanel.OnImguiRender();

        Notifications::Render(Application::GetDelta());
}

//bool
//EditorLayer::OnRemoveEntity(RemoveEntityEvent& ev)
//{
//        m_GizmoPanel.ChangeEntity(Entity());
//        return true;
//}

//bool EditorLayer::OnSceneChanged(SceneChangedEvent& ev)
//{
//        m_GizmoPanel.ChangeEntity(Entity());
//        return false;
//}

void EditorLayer::OnGameStarted()
{
        //m_GizmoPanel.ChangeEntity(Entity());
        //m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kNothing);
}

void EditorLayer::OnGameEnded()
{
        //m_GizmoPanel.ChangeEntity(Entity());
        //m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kNothing);
        //m_GizmoPanel.ChangeEntity(Entity());
}

//void EditorLayer::SetSelectedEntity(Entity e)
//{
//        m_ScenePanel.SetSelectedEntity(e);
//}

Panels::ConsolePanel&
EditorLayer::GetConsole()
{
        return m_Console;
}

const glm::vec2&
EditorLayer::GetViewportSize()
{
        return m_ViewportSize;
}

const glm::vec2&
EditorLayer::GetViewportPosition()
{
        return m_ViewportPosition;
}

}  // namespace Borek
