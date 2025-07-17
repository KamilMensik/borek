// Copyright 2024-2025 <kamilekmensik@gmail.com>

#ifndef BR_PLATFORM_WINDOWS

#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Include/Events/WindowEvents.h"
#include "Include/Events/KeyEvents.h"
#include "Include/Events/MouseEvents.h"
#include "Include/Graphics/Context.h"
#include "Include/Platform/Linux/LinuxWindow.h"
#include "Include/Debug/Log.h"
#include "Include/Debug/Assert.h"

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
                                               int height)
        {
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                wn.m_Width = width;
                wn.m_Height = height;

                wn.Callback(new WindowResizeEvent(width, height));
        });
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                wn.Callback(new WindowCloseEvent);
        });
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key,
                                        int scancode, int action, int mods)
        {
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));

                switch(action) {
                case GLFW_PRESS:
                {
                        wn.Callback(new KeyPressedEvent(SCAST<KeyCode>(key)));
                        break;
                }
                case GLFW_RELEASE:
                {
                        wn.Callback(new KeyReleasedEvent(SCAST<KeyCode>(key)));
                        break;
                }
                case GLFW_REPEAT:
                {
                        wn.Callback(new KeyPressedEvent(SCAST<KeyCode>(key), true));
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
                        wn.Callback(new MouseButtonPressedEvent(SCAST<MouseButton>(button)));
                        break;
                }
                case GLFW_RELEASE:
                {
                        wn.Callback(new MouseButtonReleasedEvent(SCAST<MouseButton>(button)));
                        break;
                }
                }
        });
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset,
                                           double yoffset){
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                
                wn.Callback(new MouseScrolledEvent(xoffset, yoffset));
        });
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x,
                                              double y){
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));
                
                wn.Callback(new MouseMovedEvent(x, y));
        });
        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned c){
                Window& wn = *SCAST<Window*>(glfwGetWindowUserPointer(window));

                wn.Callback(new KeyTypedEvent(SCAST<KeyCode>(c)));
        });
}
void Window::Shutdown()
{
        glfwDestroyWindow(m_Window);
}

Time Window::GetTime()
{
        return Time(glfwGetTime());
}

} // namespace Borek

#endif
