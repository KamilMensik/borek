// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Graphics/VertexArray.h"

namespace Borek {
namespace Graphics {

class OpenGLVertexArray : public VertexArray {
public:
        OpenGLVertexArray();
        ~OpenGLVertexArray() override;

        void Bind() const override;
        void Unbind() const override;
        uint32_t VertexCount() const override;

        void AddVertexBuffer(Ref<VertexBuffer>) override;
        void SetIndexBuffer(Ref<IndexBuffer>) override;

private:
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
        uint32_t m_Id;

};

}  // namespace Graphics
}  // namespace Borek
