// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Include/Events/WindowEvents.h"
#include "Include/Events/KeyEvents.h"
#include "Include/Events/MouseEvents.h"
#include "Include/Graphics/Context.h"
#include "Include/Platform/Linux/LinuxWindow.h"
#include "Include/Log.h"

namespace Borek
{

static bool s_GLFW_Initialized = false;

static void GLFWErrorCallback(int error_code, const char* description)
{
        BOREK_ENGINE_ERROR("GLFW Error {}: {}", error_code, description);
}

Window::Window(unsigned width, unsigned height, const std::string& title,
               bool vsync) : AbstractWindow(width, height, title)
{
        Init();
        SetVsync(vsync);
}

Window::~Window()
{
        Shutdown();
}

void Window::OnUpdate()
{
        glfwPollEvents();
        Graphics::Context::Instance()->SwapBuffers();
}

void Window::SetVsync(bool value)
{
        glfwSwapInterval(value);
        m_Vsync = value;
}

bool Window::GetVsync()
{
        return m_Vsync;
}

void Window::Init()
{
        BOREK_ENGINE_INFO("Creating window {} [{}, {}]", m_Title, m_Width,
                          m_Height);

        if (!s_GLFW_Initialized) {
                bool result = glfwInit();
                BOREK_ENGINE_ASSERT(result, "Could not initialize GLFW.");
                glfwSetErrorCallback(GLFWErrorCallback);

                s_GLFW_Initialized = true;
        }

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(),
                                    nullptr, nullptr);

        Graphics::Context::Create(this);
        Graphics::Context::Instance()->Init();


        glfwSetWindowUserPointer(m_Window, this);

        // Add callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width,
                                               int heigth)
        {
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                wn.m_Width = width;
                wn.m_Height = heigth;

                WindowResizeEvent ev(width, heigth);
                wn.Callback(ev);
        });
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                WindowCloseEvent ev;
                wn.Callback(ev);
        });
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key,
                                        int scancode, int action, int mods)
        {
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));

                switch(action) {
                case GLFW_PRESS:
                {
                        KeyPressedEvent ev(SCAST<KeyCode>(key));
                        wn.Callback(ev);
                        break;
                }
                case GLFW_RELEASE:
                {
                        KeyReleasedEvent ev(SCAST<KeyCode>(key));
                        wn.Callback(ev);
                        break;
                }
                case GLFW_REPEAT:
                {
                        KeyPressedEvent ev(SCAST<KeyCode>(key), true);
                        wn.Callback(ev);
                        break;
                }
                }
        });
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button,
                                                int action, int mods) {
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));

                switch (action) {
                case GLFW_PRESS:
                {
                        MouseButtonPressedEvent ev(SCAST<MouseButton>(button));
                        wn.Callback(ev);
                        break;
                }
                case GLFW_RELEASE:
                {
                        MouseButtonReleasedEvent ev(SCAST<MouseButton>(button));
                        wn.Callback(ev);
                        break;
                }
                }
        });
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset,
                                           double yoffset){
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                
                MouseScrolledEvent ev(xoffset, yoffset);
                wn.Callback(ev);
        });
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x,
                                              double y){
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                
                MouseMovedEvent ev(x, y);
                wn.Callback(ev);
        });
        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned c){
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));

                KeyTypedEvent ev(SCAST<KeyCode>(c));
                wn.Callback(ev);
        });
}
void Window::Shutdown()
{
        glfwDestroyWindow(m_Window);
}

} // namespace Borek
