// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <GLFW/glfw3.h>

#include "Include/Input.h"
#include "Include/Application.h"

namespace Borek {

bool Input::IsKeyPressed(KeyCode key)
{
        void* window_impl = Application::Instance().GetWindow().WindowImpl();

        return glfwGetKey(SCAST<GLFWwindow*>(window_impl),
                          SCAST<unsigned>(key));
}

bool Input::IsMouseButtonPressed(MouseButton button)
{
        void* window_impl = Application::Instance().GetWindow().WindowImpl();

        return glfwGetMouseButton(SCAST<GLFWwindow*>(window_impl),
                                  SCAST<unsigned>(button));
}

std::pair<double, double> Input::GetMousePos()
{
        std::pair<double, double> res;
        void* window_impl = Application::Instance().GetWindow().WindowImpl();

        glfwGetCursorPos(SCAST<GLFWwindow*>(window_impl),
                         &res.first, &res.second);

        return res;
}

}
