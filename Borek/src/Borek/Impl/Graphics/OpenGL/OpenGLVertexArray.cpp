// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLVertexArray.h"

namespace Borek {
namespace Graphics {

OpenGLVertexArray::OpenGLVertexArray()
{
        glGenVertexArrays(1, &m_Id);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
        glDeleteVertexArrays(1, &m_Id);
}

uint32_t OpenGLVertexArray::VertexCount() const
{
        return m_IndexBuffer->GetCount();
}


void OpenGLVertexArray::Bind() const
{
        for (auto& vb: m_VertexBuffers) vb->Bind();
        m_IndexBuffer->Bind();
        glBindVertexArray(m_Id);
}

void OpenGLVertexArray::Unbind() const
{
        glBindVertexArray(0);
}

void OpenGLVertexArray::AddVertexBuffer(Ref<VertexBuffer> vbuf)
{
        glBindVertexArray(m_Id);
        vbuf->Bind();
        m_VertexBuffers.emplace_back(vbuf);
}

void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer> ibuf)
{
        glBindVertexArray(m_Id);
        m_IndexBuffer = ibuf;
        ibuf->Bind();
}

}  // namespace Graphics
}  // namespace Borek
