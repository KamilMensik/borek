// Copyright 2024-2025 <kamilekmensik@gmail.com>

#ifndef BR_PLATFORM_WINDOWS

#include <GLFW/glfw3.h>

#include "Include/Base/Input.h"
#include "Include/Base/Application.h"

namespace Borek {
        bool Input::IsKeyPressed(KeyCode key)
        {
                void* window_impl = Application::GetWindow().WindowImpl();

                return glfwGetKey(SCAST<GLFWwindow*>(window_impl),
                                  SCAST<unsigned>(key));
        }

        bool Input::IsMouseButtonPressed(MouseButton button)
        {
                void* window_impl = Application::GetWindow().WindowImpl();

                return glfwGetMouseButton(SCAST<GLFWwindow*>(window_impl),
                                          SCAST<unsigned>(button));
        }

        glm::vec2 Input::GetMousePos()
        {
                std::pair<double, double> res;
                void* window_impl = Application::GetWindow().WindowImpl();

                glfwGetCursorPos(SCAST<GLFWwindow*>(window_impl),
                                 &res.first, &res.second);

                return glm::vec2(res.first, res.second);
        }
}

#endif
