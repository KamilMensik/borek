// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Exceptions/RubyException.h"
#include <concepts>
#include <utility>

#include "Include/Base/Scene.h"
#include "Include/Graphics/FrameBuffer.h"
#include "Include/ImGui/ImGuiLayer.h"
#include "Include/Events/Event.h"
#include "Include/Events/WindowEvents.h"
#include "Include/Base/Window.h"
#include "Include/Base/LayerStack.h"
#include "Include/Engine/FZX/CGrid.h"
#include "Include/Base/Popup.h"
#include "Include/Events/MouseEvents.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Events/EntityEvents.h"
#include "Include/Events/SceneEvents.h"
#include "Include/Events/EventQueue.h"

namespace Borek {

class Application {
public:
        Application(const std::string& name = "Borek!");
        virtual ~Application();

        static void PushLayer(Layer* layer);
        static void PushOverlay(Layer* layer);
        static AbstractWindow& GetWindow();
        static std::pair<CameraComponent*, TransformComponent*> GetCamera();
        static void Shutdown();
        static ImGuiLayer& GetImguiLayer();
        static void SetScene(Ref<Scene> scene);
        static Ref<Scene> GetScene();
        static Ref<Graphics::FrameBuffer> GetFramebuffer();
        template <class T, typename ... Args> requires std::derived_from<T, Event>
        static void SendEvent(Args...args)
        {
                s_Instance->m_EventQueue.Add<T>(std::forward<Args>(args)...);
        }
        template <class T, typename ... Args> requires std::derived_from<T, Event>
        static void SendEventImmediate(Args...args)
        {
                T ev(std::forward<Args>(args)...);
                ev.Notify();
        }
        static void OpenPopup(Popup* popup);
        static RubyEngine& GetRubyEngine();
        static std::pair<glm::vec2, glm::vec2> GetMouseOffset();
        static void Log(const std::string& str);
        static FZX::CGrid& GetSpriteGrid();
        static bool IsPlaying();
        static void
        ChangeScene(const std::string& path);
        static const std::string&
        ProjectPath();
        static Time
        GetTime();
        static void
        LoadScripts();
        static float
        GetDelta();
        static uint64_t
        GetTick();
        static void
        RestartScene();

        void Run();

protected:
        static Application* s_Instance;
        static std::function<void(const std::string&)> s_LogFunc;

        Uniq<AbstractWindow> m_Window;
        LayerStack m_Layers;

        CameraComponent* m_Camera;
        TransformComponent m_CameraTransform;

        ImGuiLayer* m_ImGuiLayer;

        Ref<Scene> m_CurrentScene;
        Ref<Graphics::FrameBuffer> m_FrameBuffer;

        EventQueue m_EventQueue;
        std::vector<Popup*> m_ActivePopups = {};

        Uniq<RubyEngine> m_RubyEngine;

        FZX::CGrid m_SpriteGrid;
        std::string m_ProjectPath;

        uint64_t m_CurrentTick = 0;
        double m_Delta = 1.0f;
        float m_AspectRatio = 1.6f;
        bool m_Running = true;

        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        bool OnMouseButton(MouseButtonEvent& e);
        bool OnDestroyEntity(DestroyEntityEvent& e);
        bool OnChangeScene(ChangeSceneEvent& e);

        void GameTick();
        void RunEntityScripts(double delta);
        void HandlePhysics();
        void DrawEntities();

        virtual void OnUpdate(float delta) {}
        virtual void OnRenderBegin();
        virtual void BeforeRenderEnd() {};
        virtual void OnRenderEnd();
        virtual void OnImGuiRenderBegin() {}
        virtual void OnImguiRenderEnd() {}
        virtual bool Playing() { return true; }
        virtual void SetCamera();
        virtual void HandleEvents();
        virtual void LoadProject();
        virtual void ChangeSceneFunc(const std::string& path);

        virtual bool HandleRubyException(const RubyException& exc);

        virtual std::pair<glm::vec2, glm::vec2> GetMouseOffsetInternal();
};

}

