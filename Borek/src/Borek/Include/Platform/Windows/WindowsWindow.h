// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <GLFW/glfw3.h>

#include "Include/Base/Window.h"

namespace Borek {

class Window : public AbstractWindow {
public:
        Window(unsigned width = 1280, unsigned height = 720,
               const std::string& title = "Borek!", bool vsync = true);
        ~Window() override;
        void OnUpdate() override;
        void SetVsync(bool value) override;
        bool GetVsync() override;

        void SetCallback(const CallbackFn& callback) override
        {
                Callback = callback;
        }
        void* WindowImpl() override
        {
                return m_Window;
        }
        Time GetTime() override;

private:
        GLFWwindow* m_Window;
        CallbackFn Callback;
        bool m_Vsync;

        void Init();
        void Shutdown();
};

}  // namespace Borek
