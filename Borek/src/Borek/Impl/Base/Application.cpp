// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "glm/trigonometric.hpp"
#include <print>
#include <ranges>

#include <box2d/box2d.h>
#include <box2d/id.h>
#include <ECS/World.h>

#include "Include/Core.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Base/Application.h"
#include "Include/Events/EventCaller.h"
#include "Include/Base/Renderer2D.h"
#include "Include/Events/ApplicationEvents.h"
#include "Include/Graphics/Camera.h"
#include "Include/ImGui/ImGuiLayer.h"
#include "Include/Base/Query.h"
#include "Include/Base/Components.h"
#include "Include/Debug/Log.h"

#ifndef BR_PLATFORM_WINDOWS
        #include "Include/Platform/Linux/LinuxWindow.h"
#else
        #include "Include/Platform/Windows/WindowsWindow.h"
#endif

namespace Borek {
Application* Application::s_Instance = nullptr;

static inline double GetDelta(AbstractWindow& window) {
        static Time prev_time = window.GetTime();
        Time new_time = window.GetTime();
        double delta = new_time.Seconds() - prev_time.Seconds();
        prev_time = new_time;
        return delta;
}

constexpr static b2BodyType to_b2(RigidBody2DComponent::Type type)
{
        switch (type) {
        case RigidBody2DComponent::Type::kStatic:
                return b2BodyType::b2_staticBody;
        case RigidBody2DComponent::Type::kDynamic:
                return b2BodyType::b2_dynamicBody;
        case RigidBody2DComponent::Type::kKinematic:
                return b2BodyType::b2_kinematicBody;
        default:
                BOREK_ASSERT(false, "Invalid rigidbody2d type!");
                return b2BodyType::b2_bodyTypeCount;
        }
}

Application::Application(const std::string& name)
{
        s_Instance = this;
        m_Window.reset(new Window(1280, 720, name));
        Renderer2D::Init();
        m_Window->SetCallback(std::bind(&Application::SendEvent,
                                        std::placeholders::_1));
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        m_CurrentScene = NewRef<Scene>();

        Graphics::FrameBufferSettings fb_settings;
        fb_settings.width = 1280;
        fb_settings.height = 720;

        m_FrameBuffer = Graphics::FrameBuffer::Create(fb_settings);
}

Application::~Application()
{
        Renderer2D::Shutdown();
}

void Application::SetCamera() {
        auto query = Query<TransformComponent, CameraComponent>();
        for (auto [transform, camera] : query) {
                if (camera->is_active) {
                        m_Camera = camera;
                        m_CameraTransform = transform;
                        break;
                }
        }

        BOREK_ASSERT(m_Camera, "Couldnt find an active camera in scene");
}

void Application::OnRenderBegin()
{
        SetCamera();
        m_Camera->aspect_ratio = m_AspectRatio;
        m_FrameBuffer->Bind();
        Graphics::Renderer::Clear();
}

void Application::OnRenderEnd()
{
        m_FrameBuffer->Unbind();
}

void Application::Run()
{
        while (m_Running)
        {
                // Handle deltatime
                double delta = GetDelta(*m_Window);

                // Called before every render
                OnRenderBegin();

                BOREK_ENGINE_ASSERT(m_Camera != nullptr, "Camera not set.");
                // Begin rendering
                Renderer2D::Begin(*m_Camera, *m_CameraTransform);

                // Custom application OnUpdate call
                OnUpdate(delta);

                if (!m_Window->IsMinimized()) {
                        for (auto layer : m_Layers)
                                layer->OnUpdate(delta);
                }

                if (!IsPlaying()) {
                        auto query = Query<IDComponent, TransformComponent, RigidBody2DComponent>();
                        for (auto [id, transform, rb2d] : query) {
                                Entity e(id->ecs_id, m_CurrentScene.get());
                                if (B2_IS_NULL(rb2d->runtime_body))
                                        return;

                                b2Body_SetTransform(rb2d->runtime_body,
                                                    b2Vec2(transform->position.x,
                                                           transform->position.y),
                                                    b2MakeRot(glm::radians(transform->rotation.z)));
                        };
                }

                if (IsPlaying()) {
                        RunEntityScripts(delta);
                        int sub_step_count = 4;
                        b2World_Step(m_CurrentScene->m_PhysicsWorld, delta, sub_step_count);
                        auto query = Query<IDComponent, TransformComponent, RigidBody2DComponent>();
                        for (auto [id, transform, rb2d] : query) {
                                Entity e(id->ecs_id, m_CurrentScene.get());

                                auto pos = b2Body_GetPosition(rb2d->runtime_body);
                                transform->position.x = pos.x;
                                transform->position.y = pos.y;

                                auto rot = b2Body_GetRotation(rb2d->runtime_body);
                                transform->rotation.z = glm::degrees(b2Rot_GetAngle(rot));
                        };
                }

                HandleEvents();

                DrawEntities();

                // Flush renderer
                Renderer2D::End();

                // Called after renderer end
                OnRenderEnd();

                // Prepare ImGui for render
                m_ImGuiLayer->RenderPrepare();

                OnImGuiRenderBegin();

                for (auto layer : m_Layers) {
                        layer->OnImGuiRender();
                }

                OnImguiRenderEnd();

                // Draw ImGui
                m_ImGuiLayer->RenderFinish();

                m_Window->OnUpdate();

                // Set Camera to nullptr to force camera updating.
                m_Camera = nullptr; m_CameraTransform = nullptr;
        }
}

void Application::RunEntityScripts(double delta)
{
        for (auto [id, script] : Query<IDComponent, RubyScriptComponent>()) {
                if (!script->ruby_instance) {
                        script->Initialize(Entity(id->ecs_id, m_CurrentScene.get()));
                }

                script->OnUpdate(delta);
                if (((mrb_state*)m_RubyEngine.GetRubyVM())->exc) {
                        mrb_print_error(m_RubyEngine.GetRubyVM());
                }
        };
}

void Application::DrawEntities()
{
        Renderer2D::DrawScene(m_CurrentScene);
}

void Application::SendEventToEntities(Event& e)
{
        for (auto [script] : Query<RubyScriptComponent>()) {
                //script->instance->OnEvent(e);
        }
}

void Application::OnEvent(Event& e)
{
        using std::ranges::views::reverse;

        EventCaller ec(e);
        ec.TryCall<WindowCloseEvent>(EVENT_FN(Application::OnWindowClose));
        ec.TryCall<WindowResizeEvent>(EVENT_FN(Application::OnWindowResize));
        ec.TryCall<ComponentAddedEvent>(EVENT_FN(Application::OnComponentAdded));

        for (auto layer : reverse(m_Layers)) {
                layer->OnEvent(e);
                if (e.GetHandled())
                        break;
        }

        SendEventToEntities(e);
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
        m_Running = false;
        return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
        Graphics::Renderer::ResizeWindow(e.GetWidth(), e.GetHeight());
        m_AspectRatio = SCAST<float>(e.GetWidth()) / e.GetHeight();
        return false; }

bool Application::OnComponentAdded(ComponentAddedEvent& e)
{
        if (e.GetId() == ECS::GetId<IDComponent>() ||
            e.GetId() == ECS::GetId<TransformComponent>() ||
            e.GetId() == ECS::GetId<TagComponent>())
                return true;

        Entity entity = Entity(e.GetEntityId(), m_CurrentScene.get());
        if (e.GetId() == ECS::GetId<RigidBody2DComponent>()) {
                auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
                auto& tran = entity.GetComponent<TransformComponent>();

                b2BodyDef def = b2DefaultBodyDef();
                def.type = to_b2(rb2d.type);
                def.position = { tran.position.x, tran.position.y };
                def.rotation = b2MakeRot(glm::radians(tran.rotation.z));
                def.motionLocks.angularZ = rb2d.fixed_rotation;

                rb2d.runtime_body = b2CreateBody(m_CurrentScene->m_PhysicsWorld,
                                                 &def);

                if (entity.HasComponent<BoxCollider2DComponent>()) {
                        auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
                        auto& tran = entity.GetComponent<TransformComponent>();

                        b2Polygon poly = b2MakeBox(bc2d.size.x * tran.scale.x,
                                                   bc2d.size.y * tran.scale.y);
                        b2ShapeDef shape = b2DefaultShapeDef();
                        shape.density = bc2d.density;
                        shape.material.friction = bc2d.friction;
                        shape.material.restitution = bc2d.restitution;

                        b2BodyId body = entity.GetComponent<RigidBody2DComponent>()
                                              .runtime_body;
                        bc2d.runtime_collider = b2CreatePolygonShape(body, &shape, &poly);
                        BOREK_ENGINE_INFO("Initialized physics for entity {}", e.GetEntityId());
                }
        } else if (e.GetId() == ECS::GetId<BoxCollider2DComponent>()) {
                if (entity.HasComponent<RigidBody2DComponent>()) {
                        auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
                        auto& tran = entity.GetComponent<TransformComponent>();

                        b2Polygon poly = b2MakeBox(bc2d.size.x * tran.scale.x,
                                                   bc2d.size.y * tran.scale.y);
                        b2ShapeDef shape = b2DefaultShapeDef();
                        shape.density = bc2d.density;
                        shape.material.friction = bc2d.friction;
                        shape.material.restitution = bc2d.restitution;

                        b2BodyId body = entity.GetComponent<RigidBody2DComponent>()
                                              .runtime_body;
                        bc2d.runtime_collider = b2CreatePolygonShape(body, &shape, &poly);
                        BOREK_ENGINE_INFO("Initialized physics for entity {}", e.GetEntityId());
                }
        }

        return false;
}

/*
 *
 * Borek Application API
 *
 */

void Application::PushLayer(Layer* layer)
{
        s_Instance->m_Layers.Push(layer);
        layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
        s_Instance->m_Layers.PushOverlay(layer);
        layer->OnAttach();
}

AbstractWindow& Application::GetWindow()
{
        return *s_Instance->m_Window;
}

std::pair<CameraComponent*, TransformComponent*> Application::GetCamera()
{
        return { s_Instance->m_Camera, s_Instance->m_CameraTransform };
}

void Application::Shutdown()
{
        s_Instance->m_Running = false;
}

ImGuiLayer& Application::GetImguiLayer()
{
        return *s_Instance->m_ImGuiLayer;
}

void Application::SetScene(Ref<Scene> scene)
{
        s_Instance->m_CurrentScene = scene;
        SendEvent(new SceneChangedEvent);
}

Ref<Scene> Application::GetScene()
{
        return s_Instance->m_CurrentScene;
}

Ref<Graphics::FrameBuffer> Application::GetFramebuffer()
{
        return s_Instance->m_FrameBuffer;
}

void Application::SendEvent(Event* e)
{
        s_Instance->m_Events.emplace_back(e);
}

RubyEngine& Application::GetRubyEngine()
{
        return s_Instance->m_RubyEngine;
}


void Application::HandleEvents()
{
        for (auto event : m_Events) {
                OnEvent(*event);
                delete event;
        }

        m_Events.clear();
}

}  // namespace Borek
