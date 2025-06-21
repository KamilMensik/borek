// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Box2D/src/body.h"
#include "Include/Debug/Log.h"
#include "box2d/types.h"
#include "glm/trigonometric.hpp"
#include <box2d/box2d.h>
#include "Include/Base/Renderer2D.h"
#include "Include/Events/ApplicationEvents.h"
#include "Include/Graphics/Camera.h"
#include <ranges>

#include "Include/Core.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Base/Application.h"
#include "Include/Events/EventCaller.h"
#include "Include/ImGui/ImGuiLayer.h"

#ifndef BR_PLATFORM_WINDOWS
        #include "Include/Platform/Linux/LinuxWindow.h"
#else
        #include "Include/Platform/Windows/WindowsWindow.h"
#endif


namespace Borek {

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
Application* Application::s_Instance = nullptr;

static inline double GetDelta(AbstractWindow& window) {
        static Time prev_time = window.GetTime();
        Time new_time = window.GetTime();
        double delta = new_time.Seconds() - prev_time.Seconds();
        prev_time = new_time;
        return delta;
}

Application::Application(const std::string& name)
{
        s_Instance = this;
        m_Window.reset(new Window(1280, 720, name));
        Renderer2D::Init();

        m_Window->SetCallback(EVENT_FN(Application::OnEvent));
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
        m_CurrentScene->GetBaseQueries().camera_query.each([this](View& view) {
                auto& c = view.get<CameraComponent>();
                if (c.is_active) {
                        m_Camera = &c;
                        m_CameraTransform = &view.get<TransformComponent>();
                        return;
                }
        });

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

                if (IsPlaying()) {
                        RunEntityScripts(delta);

                        int sub_step_count = 4;
                        BOREK_ENGINE_INFO("Delta: {}", delta);
                        b2World_Step(m_CurrentScene->m_PhysicsWorld, delta, sub_step_count);
                        BaseQueries().rigidbody_query.each([this](View& view) {
                                Entity e(view.current_entity_id,
                                         m_CurrentScene.get());
                                auto& rb2d = view.get<RigidBody2DComponent>();
                                auto& tran = view.get<TransformComponent>();

                                auto pos = b2Body_GetPosition(rb2d.runtime_body);
                                tran.position.x = pos.x;
                                tran.position.y = pos.y;

                                auto rot = b2Body_GetRotation(rb2d.runtime_body);
                                tran.rotation.z = glm::degrees(b2Rot_GetAngle(rot));
                        });
                }
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
        BaseQueries().scriptable_object_query.each([this, delta](View& view){
                auto& component = view.get<ScriptComponent>();
                component.Bind(Entity(view.current_entity_id,
                                      m_CurrentScene.get()));
                component.instance->OnUpdate(delta);
        });
}

void Application::DrawEntities()
{
        BaseQueries().draw_query.each([](View view) {
                Renderer2D::DrawQuad(view.get<TransformComponent>(),
                                     view.get<SpriteComponent>(),
                                     view.current_entity_id);
        });
}

void Application::SendEventToEntities(Event& e)
{
        BaseQueries().scriptable_object_query.each([&e](View& v){
                v.get<ScriptComponent>().instance->OnEvent(e);
        });
}

void Application::OnEvent(Event& e)
{
        using std::ranges::views::reverse;

        EventCaller ec(e);
        ec.TryCall<WindowCloseEvent>(EVENT_FN(Application::OnWindowClose));
        ec.TryCall<WindowResizeEvent>(EVENT_FN(Application::OnWindowResize));

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
        return false;
}

/*
 *
 * Borek Application API
 *
 */

const Scene::BaseQueries& Application::BaseQueries()
{
        return s_Instance->m_CurrentScene->GetBaseQueries();
}

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
        SceneChangedEvent event;
        SendEvent(event);
}

Ref<Scene> Application::GetScene()
{
        return s_Instance->m_CurrentScene;
}

Ref<Graphics::FrameBuffer> Application::GetFramebuffer()
{
        return s_Instance->m_FrameBuffer;
}

void Application::SendEvent(Event &e)
{
        s_Instance->OnEvent(e);
}

void Application::OnStart()
{
        b2WorldDef wdef = b2DefaultWorldDef();
        wdef.gravity = b2Vec2(0.0f, -9.8f);
        m_CurrentScene->m_PhysicsWorld = b2CreateWorld(&wdef);
        BaseQueries().rigidbody_query.each([this](View& view){
                auto& rb2d = view.get<RigidBody2DComponent>();
                auto& tran = view.get<TransformComponent>();
                Entity current = Entity(view.current_entity_id,
                                        m_CurrentScene.get());
                BOREK_ENGINE_INFO("Creating rigidbody for entity: {}", current.GetComponent<TagComponent>().value.c_str());

                b2BodyDef def = b2DefaultBodyDef();
                def.type = to_b2(rb2d.type);
                def.position = { tran.position.x, tran.position.y };
                def.rotation = b2MakeRot(glm::radians(tran.rotation.z));
                def.motionLocks.angularZ = rb2d.fixed_rotation;

                b2BodyId body = b2CreateBody(m_CurrentScene->m_PhysicsWorld, &def);
                rb2d.runtime_body = body;

                if (current.HasComponent<BoxCollider2DComponent>()) {
                        BOREK_ENGINE_INFO("Creating boxcollider for entity: {}", current.GetComponent<TagComponent>().value.c_str());
                        auto& bc2d = current.GetComponent<BoxCollider2DComponent>();

                        b2Polygon poly = b2MakeBox(bc2d.size.x * tran.scale.x,
                                                   bc2d.size.y * tran.scale.y);
                        b2ShapeDef shape = b2DefaultShapeDef();
                        shape.density = bc2d.density;
                        shape.material.friction = bc2d.friction;
                        shape.material.restitution = bc2d.restitution;

                        bc2d.runtime_collider = b2CreatePolygonShape(body, &shape, &poly);
                }
        });
}

void Application::OnEnd()
{
        b2DestroyWorld(m_CurrentScene->m_PhysicsWorld);
        m_CurrentScene->m_PhysicsWorld.index1 = 0;
}

}  // namespace Borek
