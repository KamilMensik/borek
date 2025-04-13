#include "Include/Graphics/BufferDescriptor.h"
#include "Include/Graphics/IndexBuffer.h"
#include "Include/Graphics/VertexBuffer.h"
#include <cstddef>
#include <memory>
#include <ranges>

#include <glad/glad.h>

#include "Include/Application.h"
#include "Include/Events/EventCaller.h"
#include "Include/Core.h"
#include "Include/ImGui/ImGuiLayer.h"
#include "Include/Graphics/Shader.h"

#ifndef BR_PLATFORM_WINDOWS
        #include "Include/Platform/Linux/LinuxWindow.h"
#else
        // When windows window will be created include it here.
#endif

const std::string vertex_shader_src = R"(
        #version 460 core

        layout(location = 0) in vec3 a_Position;
        out vec3 e_Position;

        void main()
        {
                gl_Position = vec4(a_Position, 1.0);
                e_Position = a_Position;
        }
)";

const std::string fragment_shader_src = R"(
        #version 460 core

        layout(location = 0) out vec4 color;
        in vec3 e_Position;

        void main()
        {
                  color = vec4((e_Position.x * 0.5) + 0.5,
                             (e_Position.y * 0.5) + 0.5,
                             (e_Position.z * 0.5) + 0.5,
                             1.0);
        }
)";

namespace Borek {

static Graphics::Shader* shader;
static Graphics::VertexBuffer* vertex_buffer;
static Graphics::IndexBuffer* index_buffer;
static GLuint vai;

Application* Application::m_Instance = nullptr;

Application::Application()
{
        m_Instance = this;
        m_Window = std::make_unique<Window>();

        m_Window->SetCallback(BIND_EVENT_FN(Application::OnEvent));
        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        float vertixes[] = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.0f, 0.5f, 0.0f,
        };

        uint32_t indexes[] = { 0, 1, 2 };

        glGenVertexArrays(1, &vai);

        vertex_buffer = Graphics::VertexBuffer::Create(vertixes,
                                                       sizeof(vertixes));
        index_buffer = Graphics::IndexBuffer::Create(indexes,
                                                     sizeof(indexes) / sizeof(uint32_t));

        Graphics::BufferDescriptor bd = {
                { Datatype::Float3, "a_Position" },
        };

        vertex_buffer->SetBufferDescriptor(bd);

        shader = Graphics::Shader::Create(vertex_shader_src,
                                          fragment_shader_src);

}

Application::~Application()
{
}

void Application::Run()
{
        while (m_Running)
        {
                // Clear window canvas.
                glClearColor(0.0f, 0.0f, 0.0f, 1);
                glClear(GL_COLOR_BUFFER_BIT);

                glBindVertexArray(vai);
                shader->Bind();
                vertex_buffer->Bind();
                index_buffer->Bind();

                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

                // Prepare ImGui for render
                m_ImGuiLayer->RenderPrepare();

                for (auto layer : m_Layers) {
                        layer->OnUpdate();
                        layer->OnImGuiRender();
                }

                // Draw ImGui
                m_ImGuiLayer->RenderFinish();

                // Finish rendering
                m_Window->OnUpdate();
        }
}

void Application::OnEvent(Event& e)
{
        using std::ranges::views::reverse;

        EventCaller ec(e);
        ec.TryCall<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowCloseEvent));

        for (auto layer : reverse(m_Layers)) {
                layer->OnEvent(e);
                if (e.GetHandled())
                        break;
        }
}

bool Application::OnWindowCloseEvent(WindowCloseEvent& e)
{
        m_Running = false;       
        return true;
}

}  // namespace Borek
