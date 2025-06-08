// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/Events.h"
#include "Include/Base/Input.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/SceneSerializer.h"
#include "Include/Events/EventCaller.h"
#include "Include/Events/KeyEvents.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Scripting/ScriptableObject.h"
#include "glm/ext/vector_float3.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imguismo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

#include <Borek.h>
#include <Borek/Include/Base/Components.h>
#include <Borek/Include/Engine/Utils/FileUtils.h>
#include <Borek/Include/Base/Entity.h>

#include "ECS/View.h"

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
                        t.position += (glm::vec3{ Input::GetAxis(), 0.0f } * 100.0f * delta);
                }
        }
};

EditorLayer::EditorLayer(Ref<Graphics::FrameBuffer> framebuffer)
        : Borek::Layer("TestLayer"), m_Scene(std::make_shared<Scene>()),
        m_FrameBuffer(framebuffer), m_ScenePanel(m_Scene),
        m_EditorCamera(1.6f)
{
        m_UnknownPlumber = Sprite::Create(ASSET_PATH("assets/retro-mario.png"));
        m_SpriteSheet = Borek::SpriteSheet::Create(ASSET_PATH("assets/tilemap.png"));
        m_SpriteSheet->SetStep(16, 16);
        m_SpriteSheet->SetGap(1);
        m_Subsprite = Borek::SubSprite::Create(m_SpriteSheet);

        Entity player = m_Scene->NewEntity("Player")
                               .AddComponent<SpriteComponent>(m_UnknownPlumber)
                               .AddComponent<ScriptComponent>(new PlayerController());
        player.GetComponent<TransformComponent>().scale = glm::vec3(64.0f / 400.0f);

        //Entity zwei = m_Scene->NewEntity().AddComponent<SpriteComponent>(m_UnknownPlumber);
        //zwei.GetComponent<TransformComponent>().position = { 100, 100, 0 };

        m_DrawQuery = m_Scene->Query(TransformComponent::Id(), SpriteComponent::Id());
        m_CameraQuery = m_Scene->Query(TransformComponent::Id(), CameraComponent::Id());
        m_ScriptableObjectQuery = m_Scene->Query(ScriptComponent::Id());

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Segoe UI Bold.ttf"), 24.0f);
        io.FontDefault = io.Fonts->AddFontFromFileTTF(ASSET_PATH("assets/Fonts/Segoe UI.ttf"), 24.0f);
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnUpdate(float delta)
{
        if (IsPlaying()) {
                m_ScriptableObjectQuery.each([this, delta](ECS::View& view){
                        auto& component = view.get<ScriptComponent>();
                        component.Bind(Entity(view.current_entity_id, m_Scene.get()));
                        component.instance->OnUpdate(delta);
                });
        }

        /*
        //BOREK_ENGINE_TRACE("FPS: {}", 1 / delta);
        Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 64, 64 },
                             Borek::Colors::RED);
        Renderer2D::DrawQuad({ -64.0f, -64.0f }, { 64, 64 },
                             Borek::Colors::GREEN);
        Renderer2D::DrawQuad({ 0.0f, -64 }, { 64, 64 },
                             Borek::Colors::GREEN);
        Renderer2D::DrawQuad({ -64, -64 }, { 64, 64 },
                             Borek::Colors::BLUE);
        Renderer2D::DrawQuad({ -64, 0.0f }, { 64, 64 },
                             Borek::Colors::WHITE);
        Renderer2D::DrawQuad({ -128, -128 }, { 64, 64 },
                             Borek::Colors::RED);
        Renderer2D::DrawQuad({ -128, -128 }, { 64, 64 },
                             Borek::Colors::GREEN);
        Renderer2D::DrawQuad({ -64, 0.0f }, { 64, 64 },
                             Borek::Colors::BLUE);
        Renderer2D::DrawQuad({ -128, -64 }, { 64, 64 },
                             Borek::Colors::WHITE);
        Renderer2D::DrawQuad({ -64, -64 }, { 64, 64 },
                             m_UnknownPlumber);

        for (int i = 0; i < 12; i++) {
                for (int j = 0; j < 11; j++) {
                        m_Subsprite->SetRowCol(i, j);
                        Renderer2D::DrawQuad({ i * 64, j * 64 }, { 64, 64 },
                                             m_Subsprite);
                }
        }
        */

        m_DrawQuery.each([](ECS::View view) {
                Renderer2D::DrawQuad(view.get<TransformComponent>(),
                                     view.get<SpriteComponent>());
        });
}

void EditorLayer::OnEvent(Event& ev)
{
        EventCaller ec(ev);
        ec.TryCall<ScenePanelSelectedEvent>(EVENT_FN(EditorLayer::OnScenePanelSelectedEvent));
        ec.TryCall<KeyPressedEvent>(EVENT_FN(EditorLayer::OnKeyPressedEvent));
        ec.TryCall<MouseMovedEvent>(EVENT_FN(EditorLayer::OnMouseMovedEvent));

        m_ScriptableObjectQuery.each([&ev](ECS::View& v){
                v.get<ScriptComponent>().instance->OnEvent(ev);
        });
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
                                Borek::Application::Instance().Shutdown();
                        if (ImGui::MenuItem("Open")) {
                                auto path = Utils::OpenFileDialog("", ASSET_PATH());
                                LoadScene(path);
                        }
                        if (ImGui::MenuItem("Save")) {
                                auto path = Utils::SaveFileDialog("", ASSET_PATH());
                                SceneSerializer(m_Scene).Serialize(path);
                        }
                        ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        if (ImGui::Begin("Viewport")) {
                m_IsFocused = ImGui::IsWindowFocused();
                Application::Instance().GetImguiLayer().SetEventBlocking(!m_IsFocused);

                float line_height = GImGui->FontSize + GImGui->Style.FramePadding.y * 2;
                ImVec2 button_size = { line_height + 3.0f, line_height + 1.0f };
                if (ImGui::Button("Nothing", button_size))
                        m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kNothing);
                ImGui::SameLine();
                if (ImGui::Button("Translate", button_size))
                        m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kTranslate);
                ImGui::SameLine();
                if (ImGui::Button("Rotate", button_size))
                        m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kRotate);
                ImGui::SameLine();
                if (ImGui::Button("Scale", button_size))
                        m_GizmoPanel.SetMode(Panels::GizmoPanel::Mode::kScale);
                ImVec2 viewport_size = ImGui::GetContentRegionAvail();
                viewport_size.y -= button_size.y;

                uint32_t texture_id = m_FrameBuffer->GetColorAttachmentId();
                ImGui::Image(texture_id, ImVec2(m_ViewportSize.x, m_ViewportSize.y),
                             ImVec2(0, 1), ImVec2(1, 0));

                m_GizmoPanel.DrawGizmo();

                if (viewport_size.x > 0 && viewport_size.y > 0 &&
                    (m_ViewportSize.x != viewport_size.x ||
                     m_ViewportSize.y != viewport_size.y)) {
                        m_ViewportSize.x = viewport_size.x;
                        m_ViewportSize.y = viewport_size.y;

                        WITH_CHANGE(*m_FrameBuffer, {
                                m_FrameBuffer->GetSettingsM().width = m_ViewportSize.x;
                                m_FrameBuffer->GetSettingsM().height = m_ViewportSize.y;
                        })

                        Graphics::Renderer::ResizeWindow(m_ViewportSize.x,
                                                         m_ViewportSize.y);
                }

                ImGui::End();
        }
        ImGui::PopStyleVar();

        m_ScenePanel.OnImguiRender();
        m_PropertiesPanel.OnImguiRender();
        if (ImGui::Begin("Assets")) {
                ImGui::Text("Haha assets. Ahoj Jirka Kral, cislo 12");
                ImGui::End();
        }
}

bool EditorLayer::IsPlaying()
{
        return false;
}

Entity EditorLayer::GetGameCamera()
{
        Entity result;
        m_CameraQuery.each([result, this](ECS::View& view) mutable {
                if (view.get<CameraComponent>().is_active) {
                        result = Entity(view.current_entity_id, m_Scene.get());
                        return;
                }
        });

        return result;
}

bool EditorLayer::OnScenePanelSelectedEvent(ScenePanelSelectedEvent& ev)
{
        m_PropertiesPanel.ChangeEntity(ev.GetEntity());
        m_GizmoPanel.ChangeEntity(ev.GetEntity());

        return false;
}

bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& ev)
{
        if (ev.GetKeyCode() == KeyCode::P)
                SceneSerializer(m_Scene).Serialize(ASSET_PATH("assets/scenes/test_scene.yaml"));

        return false;
}

bool EditorLayer::OnMouseMovedEvent(MouseMovedEvent& ev)
{
        if (!IsPlaying() && Input::IsMouseButtonPressed(MouseButton::BUTTON_RIGHT)) {
                glm::vec2 delta = ev.GetDelta() / 400.0f;
                m_EditorCameraTransform.position.x += delta.x;
                m_EditorCameraTransform.position.y -= delta.y;
        }
        return false;
}

void EditorLayer::LoadScene(const std::string& path)
{
        m_Scene = SceneSerializer().Deserialize(path);
        m_ScenePanel = Panels::Scene(m_Scene);
        m_PropertiesPanel.ChangeEntity(Entity());

        m_DrawQuery = m_Scene->Query(TransformComponent::Id(),
                                     SpriteComponent::Id());

        m_CameraQuery = m_Scene->Query(TransformComponent::Id(),
                                       CameraComponent::Id());

        m_ScriptableObjectQuery = m_Scene->Query(ScriptComponent::Id());
}
void EditorLayer::SetGameCamera()
{
        if (IsPlaying()) {
                Entity camera = GetGameCamera();
                Application::Instance()
                        .SetCamera(&camera.GetComponent<CameraComponent>(),
                                   &camera.GetComponent<TransformComponent>());
        } else {
                Application::Instance().SetCamera(&m_EditorCamera,
                                                  &m_EditorCameraTransform);
        }

}

}  // namespace Borek
