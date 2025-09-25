// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLIndexBuffer.h"
#include "Include/Debug/Assert.h"

namespace Borek {
namespace Graphics {

OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* vertexes, uint32_t count,
                                     bool is_dynamic)
        : m_Count(count), m_MaxCount(count) {
        glCreateBuffers(1, &m_Id);
        Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t),
                     vertexes, is_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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

void OpenGLIndexBuffer::SetData(const void* data, uint32_t count)
{
        BOREK_ENGINE_ASSERT(count < m_MaxCount, "Trying to write into index buffer data that is larger than its size");
        m_Count = count;

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(uint32_t),
                        data);
}

uint32_t OpenGLIndexBuffer::GetCount() const
{
        return m_Count;
}


}  // namespace Graphics
}  // namespace Borek
