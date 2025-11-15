// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <functional>

#include <glm/ext/matrix_float4x4.hpp>

#include "Include/Graphics/Shader.h"
#include "Include/Graphics/VertexArray.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"

namespace Borek {
namespace Graphics {

struct RendererImplementation {
        std::function<void()> clear_func;
        std::function<void(const Graphics::VertexArray&)> add_func;
        std::function<void(unsigned, unsigned, unsigned, unsigned)>
        resize_window_func;
};

class Renderer {
public:
        static void Begin(CameraComponent& cam,
                          TransformComponent& transform);

        static void End();

        static inline void Clear()
        {
                s_Clear();
        }
        static inline void Add(const Graphics::VertexArray& va)
        {
                va.Bind();
                s_Add(va);
        }
        static inline void ResizeWindow(unsigned width, unsigned height)
        {
                s_ResizeWindow(0, 0, width, height);
        }
        static inline void ResizeWindow(unsigned x, unsigned y,
                                        unsigned width, unsigned height)
        {
                s_ResizeWindow(x, y, width, height);
        }
        static void SetShader(Ref<Shader>& shader);
        static void ChangeImplementation(const RendererImplementation& impl);

private:
        static std::function<void()> s_Clear;
        static std::function<void(const Graphics::VertexArray&)> s_Add;
        static std::function<void(unsigned, unsigned, unsigned, unsigned)>
        s_ResizeWindow;
        static Ref<Shader> s_CurrentShader;
        static const glm::mat4* s_CameraProjectionView;
};

}  // namespace Graphics
}  // namespace Borek
