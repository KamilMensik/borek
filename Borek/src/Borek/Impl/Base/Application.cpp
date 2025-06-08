// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Renderer2D.h"
#include "Include/Debug/Log.h"
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

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name) : m_Time(0)
{
        s_Instance = this;
        m_Window.reset(new Window(1280, 720, name));
        Renderer2D::Init();

        m_Window->SetCallback(EVENT_FN(Application::OnEvent));
        m_Time = m_Window->GetTime();
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
        Renderer2D::Shutdown();
}

void Application::Run()
{
        while (m_Running)
        {
                Time new_time = m_Window->GetTime();
                double delta = new_time.Seconds() - m_Time.Seconds();
                m_Time = new_time;

                // Prepare ImGui for render
                m_ImGuiLayer->RenderPrepare();

                OnRenderBegin();
                // Clear window canvas.
                Graphics::Renderer::Clear();

                BOREK_ENGINE_ASSERT(m_Camera != nullptr, "Camera not set.");
                // Begin rendering
                Renderer2D::Begin(*m_Camera, *m_CameraTransform);

                // Custom application OnUpdate call
                OnUpdate(delta);

                for (auto layer : m_Layers) {
                        if (!m_Window->IsMinimized())
                                layer->OnUpdate(delta);

                        layer->OnImGuiRender();
                }

                // Flush renderer
                Renderer2D::End();
                OnRenderEnd();

                // Draw ImGui
                m_ImGuiLayer->RenderFinish();
                OnImguiRenderEnd();

                m_Window->OnUpdate();

                // Set Camera to nullptr to force camera updating.
                m_Camera = nullptr; m_CameraTransform = nullptr;
        }
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

}  // namespace Borek
