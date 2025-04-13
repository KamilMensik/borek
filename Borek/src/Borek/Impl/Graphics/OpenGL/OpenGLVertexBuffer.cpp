// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glad/glad.h>
#include <iostream>

#include "Include/Graphics/OpenGL/OpenGLVertexBuffer.h"

namespace Borek {
namespace Graphics {

uint32_t GetOpenGLType(Datatype d) {
        switch (d) {
        case Datatype::Float:
        case Datatype::Float2:
        case Datatype::Float3:
        case Datatype::Float4:
                        return GL_FLOAT;
        }

        return 0;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertexes, uint32_t size) {
        glGenBuffers(1, &m_Id);
        glBindBuffer(GL_ARRAY_BUFFER, m_Id);
        glBufferData(GL_ARRAY_BUFFER, size, vertexes, GL_STATIC_DRAW);
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
                                      0,
                                      (void*)el.offset);
                /*
                glVertexAttribFormat(i, el.Count(),
                                     GetOpenGLType(el.datatype),
                                     el.is_normalized ? GL_TRUE : GL_FALSE,
                                     el.offset);
                glVertexAttribBinding(i, 0);
                */
        }
}

void OpenGLVertexBuffer::Unbind()
{
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}  // namespace Graphics
}  // namespace Borek
