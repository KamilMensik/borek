// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <stb/image.h>
#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLTexture.h"
#include "Include/Debug/Assert.h"

namespace Borek {
namespace Graphics {

OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
{
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* img = stbi_load(path.c_str(), &width, &height,
                                 &channels, STBI_default);
        BOREK_ASSERT(img, "Image could not be loaded");
        m_Height = height;
        m_Width = width;

        GLenum internal_format, format;
        switch (channels) {
        case STBI_rgb:
                internal_format = GL_RGB8;
                format = GL_RGB;
                break;
        case STBI_rgb_alpha:
                internal_format = GL_RGBA8;
                format = GL_RGBA;
                break;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
        glTextureStorage2D(m_Id, 1, internal_format, m_Width, m_Height);

        glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_Id, 0, 0, 0, m_Width, m_Height, format,
                            GL_UNSIGNED_BYTE, img);

        stbi_image_free(img);
}

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height,
                                 const uint8_t* data)
{
        m_Height = height;
        m_Width = width;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
        glTextureStorage2D(m_Id, 1, GL_RGBA8, width, height);

        glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_Id, 0, 0, 0, width, height, GL_RGBA,
                            GL_UNSIGNED_BYTE, data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
        glDeleteTextures(1, &m_Id);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
        glBindTextureUnit(slot, m_Id);
}

uint32_t OpenGLTexture2D::GetId() const
{
        return m_Id;
}

}  // namespace Graphics
}  // namespace Borek
