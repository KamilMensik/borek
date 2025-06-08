// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Graphics/FrameBuffer.h"
namespace Borek {
namespace Graphics {

class OpenGLFrameBuffer : public FrameBuffer {
public:
        OpenGLFrameBuffer(const FrameBufferSettings& settings);
        ~OpenGLFrameBuffer() override;
        void OnChange() override;
        void Bind() override;
        void Unbind() override;
        uint32_t GetColorAttachmentId() override { return m_ColorAttachment; }

private:
        uint32_t m_ColorAttachment;
        uint32_t m_DepthAttachment;
};

}  // namespace Graphics
}  // namespace Borek
