// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLRendererImpl.h"

namespace Borek {
namespace Graphics {

static void _clear_func()
{
        glClearColor(0.0f, 0.0f, 0.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);
}

static void _add_func(const Graphics::VertexArray& va)
{
        glDrawElements(GL_TRIANGLES, va.VertexCount(),
                       GL_UNSIGNED_INT, nullptr);
}

static void _resize_window_func(unsigned x, unsigned y,
                                unsigned width, unsigned height)
{
        glViewport(x, y, width, height);
}

const RendererImplementation OpenGLRendererImpl = {
        .clear_func = _clear_func,
        .add_func = _add_func,
        .resize_window_func = _resize_window_func,
};

} // namespace Graphics
}  // namespace Borek
