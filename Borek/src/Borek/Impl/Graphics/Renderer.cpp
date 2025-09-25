// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Graphics/Renderer.h"
#include "Impl/Graphics/OpenGL/OpenGLRendererImpl.cpp"
#include "Include/Debug/Log.h"

namespace Borek {
namespace Graphics {

void Renderer::Begin(CameraComponent& camera, TransformComponent& transform)
{
        s_CameraProjectionView = &camera.GetViewProjectionMatrix(transform);
}

void Renderer::End()
{
}

std::function<void()> Renderer::s_Clear = OpenGLRendererImpl.clear_func;
std::function<void(const Graphics::VertexArray&)> Renderer::s_Add =
        OpenGLRendererImpl.add_func;
std::function<void(unsigned, unsigned, unsigned, unsigned)>
Renderer::s_ResizeWindow = OpenGLRendererImpl.resize_window_func;

Ref<Shader> Renderer::s_CurrentShader = nullptr;
const glm::mat4* Renderer::s_CameraProjectionView = nullptr;

void Renderer::ChangeImplementation(const RendererImplementation& impl)
{
        s_Clear = impl.clear_func;
        s_Add = impl.add_func;
        s_ResizeWindow = impl.resize_window_func;
}

void Renderer::SetShader(Ref<Shader>& shader)
{
        if (s_CurrentShader != shader) {
                shader->Bind();
                s_CurrentShader = shader;
        }

        s_CurrentShader->SetUniform("u_ProjectionView",
                                    *s_CameraProjectionView);
}

}  // namespace Graphics
}  // namespace Borek
