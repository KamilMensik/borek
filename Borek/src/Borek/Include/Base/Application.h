#pragma once

#include "Include/Base/Components.h"
#include "Include/ImGui/ImGuiLayer.h"
#include "Include/Events/Event.h"
#include "Include/Events/WindowEvents.h"
#include "Include/Base/Window.h"
#include "Include/Base/LayerStack.h"
#include <utility>

namespace Borek {

class Application
{
public:
        Application(const std::string& name = "Borek!");
        virtual ~Application();

        void Run();
        virtual void OnUpdate(float delta) {}
        // Set camera here every frame.
        virtual void OnRenderBegin() {}
        virtual void OnRenderEnd() {}
        virtual void OnImguiRenderEnd() {}

        void OnEvent(Event& e);

        inline void PushLayer(Layer* layer)
        {
                m_Layers.Push(layer);
                layer->OnAttach();
        }
        inline void PushOverlay(Layer* layer)
        {
                m_Layers.PushOverlay(layer);
                layer->OnAttach();
        }

        inline AbstractWindow& GetWindow()
        {
                return *m_Window;
        }
        inline static Application& Instance() { return *s_Instance; }
        // Needs to be set every tick. Otherwise gets deleted.
        inline void SetCamera(CameraComponent* cam,
                              TransformComponent* transform)
        {
                cam->aspect_ratio = m_AspectRatio;
                m_Camera = cam;
                m_CameraTransform = transform;
        }

        inline std::pair<CameraComponent*, TransformComponent*> GetCamera()
        {
                return { m_Camera, m_CameraTransform };
        }

        inline void Shutdown() { m_Running = false; }
        inline ImGuiLayer& GetImguiLayer() { return *m_ImGuiLayer; }

private:
        static Application* s_Instance;

        LayerStack m_Layers;
        Uniq<AbstractWindow> m_Window;
        CameraComponent* m_Camera;
        TransformComponent* m_CameraTransform;
        ImGuiLayer* m_ImGuiLayer;
        Time m_Time;
        float m_AspectRatio = 1.6f;
        bool m_Running = true;

        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
};

}

