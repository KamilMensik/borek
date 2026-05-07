// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Components/ZIndexComponent.h"
#include "Include/Engine/FZX/LTDGrid/SmallList.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <ECS/World.h>
#include <glm/trigonometric.hpp>
#include <mruby.h>
#include <mruby/hash.h>

#include "Include/Base/Random.h"
#include "Include/Debug/WithChange.h"
#include "Include/Engine/EntityInitializer.h"
#include "Include/Engine/Exceptions/RubyException.h"
#include "Include/Engine/ParticleSystem.h"
#include "Include/Core.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Base/Application.h"
#include "Include/Base/TransformCache.h"
#include "Include/Graphics/Camera.h"
#include "Include/ImGui/ImGuiLayer.h"
#include "Include/Base/Query.h"
#include "Include/Debug/Assert.h"
#include "Include/Components/IDComponent.h"
#include "Include/Components/RubyScriptComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Scripting/Ruby/Modules/RBChangesExporter.h"
#include "Include/Base/Project.h"
#include "Include/Events/EntityEvents.h"
#include "Include/Base/Sound.h"
#include "Include/Components/FZXComponents.h"
#include "Include/Components/SoundplayerComponent.h"
#include "Include/Debug/Log.h"
#include "Include/Drawing/Scene.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SceneAsset.h"
#include "Include/Engine/EntityUninitializer.h"
#include "Include/Engine/SceneSerializer.h"
#include "Include/Engine/ZIndexAssigner.h"
#include "Include/Events/SceneEvents.h"
#include "Include/Events/MouseEvents.h"
#include "Include/Drawing/BatchRenderer.h"
#include "Include/Engine/Assets/ScriptAsset.h"
#include "Include/Engine/Utils/PathHelpers.h"

#ifndef BR_PLATFORM_WINDOWS
        #include "Include/Platform/Linux/LinuxWindow.h"
#else
        #include "Include/Platform/Windows/WindowsWindow.h"
#endif

namespace Borek {
Application* Application::s_Instance = nullptr;

namespace fs = std::filesystem;

static Set<uint32_t> destroyed_entity_ids;
static Map<uint32_t, Asset<ScriptAsset>> loaded_scripts_map;
static mrb_value on_finished_hash;
static mrb_value sender_key;

static void
load_scripts_from_dir(const fs::path& path)
{
        for (auto& file : fs::directory_iterator(path)) {
                if (file.is_regular_file() && file.path().extension() == ".scr") {
                        const std::string rel_path = Utils::Path::ToRelative(file.path());
                        auto ass = AssetManager::Get<ScriptAsset>(rel_path);
                        loaded_scripts_map[ass.GetId()] = ass;
                } else if (file.is_directory()) {
                        load_scripts_from_dir(file.path());
                }
        }
}

static inline double
get_delta(AbstractWindow& window) {
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
        Random::Init();
        Input::Init();
        m_RubyEngine = NewUniq<RubyEngine>();

        m_Window.reset(new Window(1920, 1080, name));
        Drawing::BatchRenderer::Init();
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        SetScene(NewRef<Scene>());

        Graphics::FrameBufferSettings fb_settings;
        fb_settings.width = m_Window->GetWidth();
        fb_settings.height = m_Window->GetHeight();

        m_FrameBuffer = Graphics::FrameBuffer::Create(fb_settings);

        SoundEngine::Init();

        WindowCloseEvent::AddListener(EVENT_FN(OnWindowClose));
        WindowResizeEvent::AddListener(EVENT_FN(OnWindowResize));
        MouseButtonEvent::AddListener(EVENT_FN(OnMouseButton));
        DestroyEntityEvent::AddListener(EVENT_FN(OnDestroyEntity));
        ChangeSceneEvent::AddListener(EVENT_FN(OnChangeScene));
}

Application::~Application()
{
        Drawing::BatchRenderer::Deinitialize();
        m_CurrentScene = nullptr;
        m_RubyEngine = nullptr;
        loaded_scripts_map.clear();
        AssetManager::Clean(true);
        SoundEngine::Deinitialize();
}

void
Application::SetCamera() {
        auto query = Query<IDComponent, TransformComponent, CameraComponent>();
        for (auto [id, transform, camera] : query) {
                if (camera->is_active) {
                        m_Camera = camera;
                        m_CameraTransform = Entity(id->ecs_id).GlobalTransform();
                        m_Camera->Recalculate(m_CameraTransform);
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
        m_FrameBuffer->Blit();
        m_FrameBuffer->Unbind();
}

void
Application::Run()
{
        LoadProject();
        on_finished_hash = mrb_hash_new(m_RubyEngine->GetRubyVM());
        sender_key = mrb_symbol_value(mrb_intern_cstr(m_RubyEngine->GetRubyVM(),
                                                      "sender"));

        while (m_Running) {
                try {
                        GameTick();
                        m_CurrentTick++;
                } catch (const RubyException& e) {
                        if (!HandleRubyException(e))
                                return;
                }
        }
}

void
Application::GameTick()
{
        destroyed_entity_ids.clear();

        HandleEvents();

        // Handle deltatime
        m_Delta = get_delta(*m_Window);
        if (m_Delta > 0.2f)
                return;

        // Custom application OnUpdate call
        OnUpdate(m_Delta);

        if (!m_Window->IsMinimized()) {
                for (auto layer : m_Layers)
                        layer->OnUpdate(m_Delta);
        }

        if (IsPlaying()) {
                RunEntityScripts(m_Delta);
                RBModules::ChangesExporter::Upload(m_RubyEngine->GetRubyVM());

                HandlePhysics();
        }

        SetCamera();

        // Called before every render
        OnRenderBegin();
        SetCamera();
        BOREK_ENGINE_ASSERT(m_Camera != nullptr, "Camera not set.");

        // Begin rendering
        Drawing::BatchRenderer::Begin(*m_Camera, m_CameraTransform);
        DrawEntities();

        ParticleSystem::OnUpdate(m_Delta);
        ParticleSystem::Render();

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
}

void
Application::RunEntityScripts(double delta)
{
        for (auto [id, script] : Query<IDComponent, RubyScriptComponent>()) {
                if (!script->ruby_instance) {
                        script->Initialize(Entity(id->ecs_id));
                }

                script->OnUpdate(delta);
                if (((mrb_state*)m_RubyEngine->GetRubyVM())->exc) {
                        mrb_print_error(m_RubyEngine->GetRubyVM());
                }
        };

        for (auto [id, sound] : Query<IDComponent, SoundPlayerComponent>()) {
                if (sound->Finished()) {
                        mrb_hash_set(m_RubyEngine->GetRubyVM(),
                                     on_finished_hash,
                                     sender_key,
                                     { Entity(id->ecs_id).GetRubyNode() });

                        sound->on_finished->Call(on_finished_hash.w);
                }
        }
}

void
Application::HandlePhysics()
{
        auto& fzxwrld = GetScene()->GetPhysicsWorld();

        for (auto& [id, body] : Query<IDComponent, BodyComponent>()) {
                auto& tc = TransformCache::GetTransform(id->ecs_id);
                GetScene()->GetPhysicsWorld().Update(
                        id->ecs_id,
                        *body,
                        tc.position,
                        scale_tof(tc.scale),
                        tc.rotation);
        }

        for (auto& [id, area] : Query<IDComponent, AreaComponent>()) {
                auto& tc = TransformCache::GetTransform(id->ecs_id);
                GetScene()->GetPhysicsWorld().Update(
                        id->ecs_id,
                        *area,
                        tc.position,
                        scale_tof(tc.scale),
                        tc.rotation);

                auto collisions = fzxwrld.GetCollisions(id->ecs_id, *area);
                std::vector<uint32_t> old_collisions = *area->colliding_with;
                area->colliding_with->clear();

                for (uint32_t cid : collisions) {
                        auto it = std::find(old_collisions.begin(),
                                            old_collisions.end(), cid);
                        
                        if (it == old_collisions.end()) {
                                area->colliding_with->emplace_back(cid);
                                area->OnEntityEntered(id->ecs_id, cid);
                        } else {
                                *it = old_collisions.back();
                                old_collisions.pop_back();
                                area->colliding_with->emplace_back(cid);
                        }
                }

                for (uint32_t exited_id : old_collisions) {
                        area->OnEntityExited(id->ecs_id, exited_id);
                }
        }
}

void
Application::DrawEntities()
{
        ZIndexAssigner::Assign();
        Drawing::Scene::Draw();

        if (IsPlaying())
                return;

        FZX::Body body;
        body.collider_type = FZX::ColliderType::Rectangle;
        body.body_type = FZX::BodyType::Static;

        for (auto [id, sprite] : Query<IDComponent, SpriteComponent>()) {
                auto tc = TransformCache::GetTransform(id->ecs_id);
                tc.position -= m_CameraTransform.position;
                body.rc.size_x = sprite->size_x;
                body.rc.size_y = sprite->size_y;

                if (!m_SpriteGrid.Contains(id->ecs_id)) {
                        m_SpriteGrid.Insert(id->ecs_id, body,
                                            tc.position,
                                            scale_tof(tc.scale),
                                            tc.rotation);
                } else {
                        m_SpriteGrid.Update(id->ecs_id, body,
                                            tc.position,
                                            scale_tof(tc.scale),
                                            tc.rotation);
                }
        }

        for (auto [id, sprite] : Query<IDComponent, AnimatedSpriteComponent>()) {
                auto tc = TransformCache::GetTransform(id->ecs_id);
                tc.position -= m_CameraTransform.position;
                body.rc.size_x = sprite->size_x;
                body.rc.size_y = sprite->size_y;

                if (!m_SpriteGrid.Contains(id->ecs_id)) {
                        m_SpriteGrid.Insert(id->ecs_id, body,
                                            tc.position,
                                            tc.scale,
                                            tc.rotation);
                } else {
                        m_SpriteGrid.Update(id->ecs_id, body,
                                            tc.position,
                                            tc.scale,
                                            tc.rotation);
                }
        }
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
        WITH_CHANGE(*m_FrameBuffer, {
                auto& settings = m_FrameBuffer->GetSettingsM();
                settings.width = e.GetWidth();
                settings.height = e.GetHeight();
        });
        return false;
}

bool
Application::OnMouseButton(MouseButtonEvent& e)
{
        //if (!e.IsPressed() && !IsPlaying())
        //        return false;

        //glm::vec2 pos = Input::GetMouseWorldPos();

        //glm::vec2 pos;
        //for (auto clicked : m_CurrentScene->GetPhysicsWorld().GetCollisions(pos)) {
        //        BOREK_ENGINE_INFO("Clicked: {}, at pos: [{}, {}]", clicked, pos.x, pos.y);
        //}

        return false;
}

bool
Application::OnDestroyEntity(DestroyEntityEvent& e)
{
        if (destroyed_entity_ids.contains(e.GetEntity()))
                return true;

        destroyed_entity_ids.insert(e.GetEntity());
        e.GetEntity().Delete();
        return false;
}

bool
Application::OnChangeScene(ChangeSceneEvent& e)
{
        ChangeScene(e.GetScenePath());
        return true;
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
        if (s_Instance->m_CurrentScene) {
                Ref<Scene> old_scene = s_Instance->m_CurrentScene;
                old_scene->Uninitialize();
                TransformCache::Reset();
        }

        s_Instance->m_CurrentScene = scene;
        scene->Init();
        SendEventImmediate<SceneChangedEvent>();
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
Application::OpenPopup(Popup* popup)
{
        s_Instance->m_ActivePopups.emplace_back(popup);
}

RubyEngine&
Application::GetRubyEngine()
{
        return *s_Instance->m_RubyEngine;
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

bool
Application::IsPlaying()
{
        return s_Instance->Playing();
}

void
Application::ChangeScene(const std::string& path)
{
        auto scene = s_Instance->m_CurrentScene;
        s_Instance->ChangeSceneFunc(path);
}

const std::string&
Application::ProjectPath()
{
        return s_Instance->m_ProjectPath;
}

Time
Application::GetTime()
{
        return GetWindow().GetTime();
}

void
Application::LoadScripts()
{
        if (s_Instance->m_ProjectPath.empty())
                return;

        load_scripts_from_dir(s_Instance->m_ProjectPath);
}
float
Application::GetDelta()
{
        return s_Instance->m_Delta;
}

uint64_t
Application::GetTick()
{
        return s_Instance->m_CurrentTick;
}

void
Application::RestartScene()
{
        ChangeScene(s_Instance->m_CurrentScene->GetPath());
}

void
Application::HandleEvents()
{
        m_EventQueue.Handle();
}

void
Application::LoadProject()
{
        m_ProjectPath = std::filesystem::current_path();
        std::filesystem::path ppath(m_ProjectPath);
        ppath /= "project.bproj";

        if (!std::filesystem::exists(ppath)) {
                std::cout << "Could not find project file. Exitting.";
                exit(1);
        }

        Project::Deserialize(ppath);
        ChangeScene(Utils::Path::FromRelative(Project::Instance().start_scene_path));
        LoadScripts();
}
void
Application::ChangeSceneFunc(const std::string& path)
{
        Application::SetScene(NewRef<Scene>());

        SceneAsset ass;
        ass.Deserialize(path);
        SceneSerializer(m_CurrentScene).Deserialize(ass.scene_path);
        m_CurrentScene->Initialize();
}

bool
Application::HandleRubyException(const RubyException& exc)
{
        BOREK_ENGINE_ERROR("Ruby Error!");
        mrb_print_error(m_RubyEngine->GetRubyVM());
        return false;
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
