#pragma once

#include <memory>

#include "Include/ImGui/ImGuiLayer.h"

#include "Include/Events/Event.h"
#include "Include/Events/WindowEvents.h"
#include "Include/Window.h"
#include "Include/LayerStack.h"

namespace Borek {

class Application
{
public:
        Application();
        virtual ~Application();

        void Run();

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
        inline static Application& Instance() { return *m_Instance; }

private:
        static Application* m_Instance;
        LayerStack m_Layers;
        std::unique_ptr<AbstractWindow> m_Window;
        ImGuiLayer* m_ImGuiLayer;
        bool m_Running = true;

        bool OnWindowCloseEvent(WindowCloseEvent& e);
};

Application* CreateApplication();

}

