// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/TransformComponent.h"
#include "Include/Engine/Utils/GeometryUtils.h"
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
        std::pair<double, double> dres;
        auto& window = Application::GetWindow();
        void* window_impl = window.WindowImpl();

        glfwGetCursorPos(SCAST<GLFWwindow*>(window_impl),
                         &dres.first, &dres.second);

        glm::vec2 res(dres.first, dres.second);
        auto offset = Application::GetMouseOffset();
        res -= offset.first;
        res.y = offset.second.y - res.y;

        return res;
}

glm::vec2 Input::GetMousePosRelative()
{
        std::pair<double, double> dres;
        auto& window = Application::GetWindow();
        void* window_impl = window.WindowImpl();

        glfwGetCursorPos(SCAST<GLFWwindow*>(window_impl),
                         &dres.first, &dres.second);

        glm::vec2 res(dres.first, dres.second);
        auto offset = Application::GetMouseOffset();
        res -= offset.first;
        res.y = offset.second.y - res.y;
        res = glm::vec2(2) * (res / offset.second) - glm::vec2(1);

        return res;
}

glm::vec2 Input::GetMouseWorldPos()
{
        const TransformComponent& ctran = *Application::GetCamera().second;

        return ctran.position + Utils::Geometry::rotate_point(
                glm::vec2(0), Input::GetMousePosRelative() * glm::vec2(1280 / 4, 720 / 4),
                ctran.rotation
        );
}

}  // namespace Borek

#endif
