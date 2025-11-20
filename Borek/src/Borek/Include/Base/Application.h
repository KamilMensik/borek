// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/EventQueue.h"
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
        static void OpenPopup(Popup* popup);
        static RubyEngine& GetRubyEngine();
        static std::pair<glm::vec2, glm::vec2> GetMouseOffset();
        static void Log(const std::string& str);
        static FZX::CGrid& GetSpriteGrid();
        static bool IsPlaying();

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

        RubyEngine m_RubyEngine;

        FZX::CGrid m_SpriteGrid;

        float m_AspectRatio = 1.6f;
        bool m_Running = true;

        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        bool OnMouseButton(MouseButtonEvent& e);

        void RunEntityScripts(double delta);
        void DrawEntities();
        void SendEventToEntities(Event& e);

        virtual void OnUpdate(float delta) {}
        virtual void OnRenderBegin();
        virtual void BeforeRenderEnd() {};
        virtual void OnRenderEnd();
        virtual void OnImGuiRenderBegin() {}
        virtual void OnImguiRenderEnd() {}
        virtual bool Playing() { return true; }
        virtual void SetCamera();
        virtual void HandleEvents();
        virtual std::pair<glm::vec2, glm::vec2> GetMouseOffsetInternal();
};

}

