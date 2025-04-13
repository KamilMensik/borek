// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLIndexBuffer.h"

namespace Borek {
namespace Graphics {

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* vertexes, uint32_t count)
        : m_Count(count) {
        glCreateBuffers(1, &m_Id);
        Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), vertexes, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
        glDeleteBuffers(1, &m_Id);
}

void OpenGLIndexBuffer::Bind() const
{
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
}

void OpenGLIndexBuffer::Unbind() const
{
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}  // namespace Graphics
}  // namespace Borek
