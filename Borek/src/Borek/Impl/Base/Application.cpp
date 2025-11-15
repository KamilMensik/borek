// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Input.h"
#include "Include/Base/Sound.h"
#include "Include/Debug/Log.h"
#include "Include/Drawing/Scene.h"
#include "Include/Engine/ZIndexAssigner.h"
#include "Include/Events/MouseEvents.h"
#include <iostream>
#include <ranges>

#include <ECS/World.h>
#include <glm/trigonometric.hpp>

#include "Include/Core.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Base/Application.h"
#include "Include/Events/EventCaller.h"
#include "Include/Events/ApplicationEvents.h"
#include "Include/Graphics/Camera.h"
#include "Include/ImGui/ImGuiLayer.h"
#include "Include/Base/Query.h"
#include "Include/Debug/Assert.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/RubyScriptComponent.h"
#include "Include/Components/SpriteComponent.h"

#include "Include/Drawing/BatchRenderer.h"

#ifndef BR_PLATFORM_WINDOWS
        #include "Include/Platform/Linux/LinuxWindow.h"
#else
        #include "Include/Platform/Windows/WindowsWindow.h"
#endif

namespace Borek {
Application* Application::s_Instance = nullptr;

static inline double
GetDelta(AbstractWindow& window) {
        static Time prev_time = window.GetTime();
        Time new_time = window.GetTime();
        double delta = new_time.Seconds() - prev_time.Seconds();
        prev_time = new_time;
        return delta;
}

Application::Application(const std::string& name)
        : m_SpriteGrid(-1536, -864, 1536, 864, 96, 96)
{
        s_Instance = this;

        m_Window.reset(new Window(1920, 1080, name));
        Drawing::BatchRenderer::Init();
        m_Window->SetCallback(std::bind(&Application::SendEvent,
                                        std::placeholders::_1));
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        SetScene(NewRef<Scene>());

        Graphics::FrameBufferSettings fb_settings;
        fb_settings.width = 320;
        fb_settings.height = 180;

        m_FrameBuffer = Graphics::FrameBuffer::Create(fb_settings);

        SoundEngine::Init();
}

Application::~Application()
{
        Drawing::BatchRenderer::Deinitialize();
        SoundEngine::Deinitialize();
}

void
Application::SetCamera() {
        auto query = Query<IDComponent, TransformComponent, CameraComponent>();
        for (auto [id, transform, camera] : query) {
                if (camera->is_active) {
                        m_Camera = camera;
                        m_CameraTransform = Entity(id->ecs_id).GlobalTransform();
                        m_Camera->aspect_ratio = m_AspectRatio;
                        break;
                }
        }

        BOREK_ASSERT(m_Camera, "Couldnt find an active camera in scene");
}

void
Application::OnRenderBegin()
{
        m_FrameBuffer->Bind();
        Graphics::Renderer::Clear();
}

void
Application::OnRenderEnd()
{
        m_FrameBuffer->Unbind();
}

void
Application::Run()
{
        while (m_Running)
        {
                // Handle deltatime
                double delta = GetDelta(*m_Window);
                if (delta > 0.2f)
                        continue;

                // Custom application OnUpdate call
                OnUpdate(delta);

                if (!m_Window->IsMinimized()) {
                        for (auto layer : m_Layers)
                                layer->OnUpdate(delta);
                }


                //if (!IsPlaying()) {
                //        auto query = Query<IDComponent, TransformComponent, RigidBody2DComponent>();
                //        for (auto [id, transform, rb2d] : query) {
                //                Entity e(id->ecs_id);
                //                if (B2_IS_NULL(rb2d->runtime_body))
                //                        return;

                //                b2Body_SetTransform(rb2d->runtime_body,
                //                                    b2Vec2(transform->position.x,
                //                                           transform->position.y),
                //                                    b2MakeRot(glm::radians(transform->rotation)));
                //        };
                //}

                if (IsPlaying()) {
                        RunEntityScripts(delta);

                        TransformComponent global_tran;
                        GetScene()->TraverseScene([&global_tran](Entity e) {
                                auto& transform = e.Transform();
                                global_tran += transform;
                                if (e.HasComponent<FZX::BodyComponent>()) {
                                        auto& body = e.GetComponent<FZX::BodyComponent>();

                                        GetScene()->GetPhysicsWorld().Update(e, body, global_tran.position,
                                                                             global_tran.scale);
                                }
                        }, [&global_tran](Entity e) {
                                global_tran -= e.GetComponent<TransformComponent>();
                        });

                        for (auto& [id, body] : Query<IDComponent, FZX::BodyComponent>()) {
                                auto collisions = GetScene()->GetPhysicsWorld().GetCollisions(id->ecs_id, *body);
                        }
                }


                HandleEvents();


                // Called before every render
                OnRenderBegin();
                SetCamera();
                BOREK_ENGINE_ASSERT(m_Camera != nullptr, "Camera not set.");

                // Begin rendering
                Drawing::BatchRenderer::Begin(*m_Camera, m_CameraTransform);
                DrawEntities();

                BeforeRenderEnd();

                // Flush renderer
                Drawing::BatchRenderer::End();

                // Called after renderer end
                OnRenderEnd();

                // Prepare ImGui for render
                m_ImGuiLayer->RenderPrepare();

                OnImGuiRenderBegin();

                for (auto layer : m_Layers) {
                        layer->OnImGuiRender();
                }

                for (int i = 0; auto popup : m_ActivePopups) {
                        if (popup->Tick()) {
                                delete popup;
                                m_ActivePopups.erase(m_ActivePopups.begin() + i);
                        }
                        i++;
                }

                OnImguiRenderEnd();

                // Draw ImGui
                m_ImGuiLayer->RenderFinish();

                m_Window->OnUpdate();

                // Set Camera to nullptr to force camera updating.
                m_Camera = nullptr;
        }
}

void
Application::RunEntityScripts(double delta)
{
        for (auto [id, script] : Query<IDComponent, RubyScriptComponent>()) {
                if (!script->ruby_instance) {
                        script->Initialize(Entity(id->ecs_id));
                }

                script->OnUpdate(delta);
                if (((mrb_state*)m_RubyEngine.GetRubyVM())->exc) {
                        mrb_print_error(m_RubyEngine.GetRubyVM());
                }
        };
}

void
Application::DrawEntities()
{
        for (auto [id, sprite] : Query<IDComponent, SpriteComponent>()) {
                const auto global_tran = Entity(id->ecs_id).GlobalTransform();
                const glm::vec2& res = global_tran.position;
                const glm::vec2& size = global_tran.scale;
                SimdVec4f new_rect(res.x, res.y, res.x + size.x, res.y + size.y);

                if (!m_SpriteGrid.Contains(id->ecs_id)) {
                        m_SpriteGrid.Insert(id->ecs_id, new_rect);
                } else {
                        m_SpriteGrid.Update(id->ecs_id, new_rect);
                }
        }

        ZIndexAssigner::Assign();
        Drawing::Scene::Draw();
}

void
Application::SendEventToEntities(Event& e)
{
        for (auto [script] : Query<RubyScriptComponent>()) {
                //script->instance->OnEvent(e);
        }
}

void
Application::OnEvent(Event& e)
{
        using std::ranges::views::reverse;

        EventCaller ec(e);
        ec.TryCall<WindowCloseEvent>(EVENT_FN(Application::OnWindowClose));
        ec.TryCall<WindowResizeEvent>(EVENT_FN(Application::OnWindowResize));
        ec.TryCall<MouseButtonPressedEvent>(EVENT_FN(Application::OnMouseButtonPressed));

        for (auto layer : reverse(m_Layers)) {
                layer->OnEvent(e);
                if (e.GetHandled())
                        break;
        }

        SendEventToEntities(e);
}

bool
Application::OnWindowClose(WindowCloseEvent& e)
{
        m_Running = false;
        return true;
}

bool
Application::OnWindowResize(WindowResizeEvent& e)
{
        Graphics::Renderer::ResizeWindow(e.GetWidth(), e.GetHeight());
        m_AspectRatio = SCAST<float>(e.GetWidth()) / e.GetHeight();
        return false;
}

bool
Application::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
        if (!IsPlaying())
                return false;

        glm::vec2 pos = Input::GetMouseWorldPos();

        for (auto clicked : m_CurrentScene->GetPhysicsWorld().GetCollisions(pos)) {
                //BOREK_ENGINE_INFO("Clicked: {}, at pos: [{}, {}]", clicked, pos.x, pos.y);
        }

        return false;
}

/*
 *
 * Borek Application API
 *
 */

void
Application::PushLayer(Layer* layer)
{
        s_Instance->m_Layers.Push(layer);
        layer->OnAttach();
}

void
Application::PushOverlay(Layer* layer)
{
        s_Instance->m_Layers.PushOverlay(layer);
        layer->OnAttach();
}

AbstractWindow&
Application::GetWindow()
{
        return *s_Instance->m_Window;
}

std::pair<CameraComponent*, TransformComponent*>
Application::GetCamera()
{
        return { s_Instance->m_Camera, &s_Instance->m_CameraTransform };
}

void
Application::Shutdown()
{
        BOREK_ENGINE_INFO("Ran shutdown");
        s_Instance->m_Running = false;
}

ImGuiLayer&
Application::GetImguiLayer()
{
        return *s_Instance->m_ImGuiLayer;
}

void
Application::SetScene(Ref<Scene> scene)
{
        s_Instance->m_CurrentScene = scene;
        scene->Init();
        SendEvent(new SceneChangedEvent);
}

Ref<Scene>
Application::GetScene()
{
        return s_Instance->m_CurrentScene;
}

Ref<Graphics::FrameBuffer>
Application::GetFramebuffer()
{
        return s_Instance->m_FrameBuffer;
}

void
Application::SendEvent(Event* e)
{
        s_Instance->m_Events.emplace_back(e);
}

void
Application::OpenPopup(Popup* popup)
{
        s_Instance->m_ActivePopups.emplace_back(popup);
}

RubyEngine&
Application::GetRubyEngine()
{
        return s_Instance->m_RubyEngine;
}

std::pair<glm::vec2, glm::vec2>
Application::GetMouseOffset()
{
        return s_Instance->GetMouseOffsetInternal();
}

void
Application::Log(const std::string& str)
{
        s_LogFunc(str);
}

FZX::CGrid&
Application::GetSpriteGrid()
{
        return s_Instance->m_SpriteGrid;
}

void
Application::HandleEvents()
{
        for (size_t i = 0; i < m_Events.size(); i++) {
                OnEvent(*m_Events[i]);
                delete m_Events[i];
        }

        m_Events.clear();
}

std::pair<glm::vec2, glm::vec2>
Application::GetMouseOffsetInternal()
{
        return { glm::vec2(0), {m_Window->GetWidth(), m_Window->GetHeight()} };
}

std::function<void(const std::string&)>
Application::s_LogFunc = [](const std::string& str) {
        std::cout << str << "\033[39m\033[49m\n";
};

}  // namespace Borek
