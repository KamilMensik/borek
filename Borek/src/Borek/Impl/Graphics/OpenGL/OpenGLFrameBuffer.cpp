// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLFrameBuffer.h"

namespace Borek {
namespace Graphics {

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSettings& settings)
        : FrameBuffer(settings)
{
        OnChange();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
        glDeleteFramebuffers(1, &m_Id);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFrameBuffer::OnChange()
{
        if (m_Id != 0) {
                glDeleteFramebuffers(1, &m_Id);
                glDeleteTextures(1, &m_ColorAttachment);
                glDeleteTextures(1, &m_DepthAttachment);
        }
        glCreateFramebuffers(1, &m_Id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Settings.width,
                     m_Settings.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_ColorAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_ColorAttachment, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Settings.width,
                       m_Settings.height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                               GL_TEXTURE_2D, m_DepthAttachment, 0);

        BOREK_ENGINE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                            "Could not generate framebuffer");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLFrameBuffer::Bind()
{
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
}

void OpenGLFrameBuffer::Unbind()
{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace Graphics
}  // namespace Borek
