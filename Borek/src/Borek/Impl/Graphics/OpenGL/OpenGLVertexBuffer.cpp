// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLVertexBuffer.h"
#include "Include/Debug/Assert.h"

namespace Borek {
namespace Graphics {

uint32_t GetOpenGLType(Datatype d) {
        switch (d) {
        case Datatype::Float:
        case Datatype::Float2:
        case Datatype::Float3:
        case Datatype::Float4:
                return GL_FLOAT;
        case Datatype::Int:
        case Datatype::Int2:
        case Datatype::Int3:
        case Datatype::Int4:
                return GL_INT;
        }

        return 0;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertexes, uint32_t size,
                                       bool is_dynamic) {
        m_Size = size;
        glGenBuffers(1, &m_Id);
        glBindBuffer(GL_ARRAY_BUFFER, m_Id);
        glBufferData(GL_ARRAY_BUFFER, size, vertexes,
                     is_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
        glDeleteBuffers(1, &m_Id);
}

void OpenGLVertexBuffer::Bind()
{
        // TODO: Use OpenGL Separate attribute format.
        glBindBuffer(GL_ARRAY_BUFFER, m_Id);
        for (unsigned i = 0; auto& el : m_Descriptor) {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, el.Count(),
                                      GetOpenGLType(el.datatype),
                                      el.is_normalized ? GL_TRUE : GL_FALSE,
                                      m_Descriptor.Size(),
                                      (void*)el.offset);
                i++;
        }
}

void OpenGLVertexBuffer::Unbind()
{
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
{
        BOREK_ENGINE_ASSERT(size < m_Size, "Trying to write into vertex buffer data that is larger than its size");
        glBindBuffer(GL_ARRAY_BUFFER, m_Id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

}  // namespace Graphics
}  // namespace Borek
