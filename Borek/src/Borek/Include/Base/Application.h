// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/ApplicationEvents.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include <utility>

#include "Include/Base/Components.h"
#include "Include/Base/Scene.h"
#include "Include/Graphics/FrameBuffer.h"
#include "Include/ImGui/ImGuiLayer.h"
#include "Include/Events/Event.h"
#include "Include/Events/WindowEvents.h"
#include "Include/Base/Window.h"
#include "Include/Base/LayerStack.h"

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
        static void SendEvent(Event* e);
        static RubyEngine& GetRubyEngine();

        void Run();

protected:
        static Application* s_Instance;

        LayerStack m_Layers;
        Uniq<AbstractWindow> m_Window;

        CameraComponent* m_Camera;
        TransformComponent* m_CameraTransform;

        ImGuiLayer* m_ImGuiLayer;

        Ref<Scene> m_CurrentScene;
        Ref<Graphics::FrameBuffer> m_FrameBuffer;

        std::vector<Event*> m_Events;

        RubyEngine m_RubyEngine;

        float m_AspectRatio = 1.6f;
        bool m_Running = true;

        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
        bool OnComponentAdded(ComponentAddedEvent& e);

        void RunEntityScripts(double delta);
        void DrawEntities();
        void SendEventToEntities(Event& e);

        virtual void OnEvent(Event& e);
        virtual void OnUpdate(float delta) {}
        virtual void OnRenderBegin();
        virtual void OnRenderEnd();
        virtual void OnImGuiRenderBegin() {}
        virtual void OnImguiRenderEnd() {}
        virtual bool IsPlaying() { return true; }
        virtual void SetCamera();
        virtual void HandleEvents();
};

}

