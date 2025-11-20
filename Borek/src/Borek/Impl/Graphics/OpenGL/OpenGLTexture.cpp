// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Assets/AssetFlags.h"
#include <cstdint>
#include <filesystem>
#include <fstream>

#include <stb/image.h>
#include <glad/glad.h>

#include "Include/Graphics/OpenGL/OpenGLTexture.h"
#include "Include/Debug/Assert.h"

namespace Borek {
namespace Graphics {

OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
{
        const std::filesystem::path p = path;

        stbi_uc* stbi_img = nullptr;
        char* img = nullptr;
        if (p.extension() != ".tex") {
                stbi_set_flip_vertically_on_load(1);
                stbi_img = stbi_load(path.c_str(), RCAST<int*>(&m_Width),
                                     RCAST<int*>(&m_Height),
                                     RCAST<int*>(&m_Channels), STBI_default);
        } else {
                std::ifstream asset(path, std::ios::binary);
                asset.read(RCAST<char*>(&m_Width), sizeof(uint32_t))
                     .read(RCAST<char*>(&m_Height), sizeof(uint32_t))
                     .read(RCAST<char*>(&m_Channels), sizeof(uint32_t));

                img = new char[m_Width * m_Height * m_Channels];
                asset.read(img, m_Width * m_Height * m_Channels);
        }

        GLenum internal_format, format;
        switch (m_Channels) {
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
                            GL_UNSIGNED_BYTE, stbi_img ? RCAST<char*>(stbi_img) : img);

        if (stbi_img)
                stbi_image_free(stbi_img);
        else
                delete[] img;
}

OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height,
                                 const uint8_t* data, int channels,
                                 uint32_t flags)
{
        m_Height = height;
        m_Width = width;
        m_Channels = channels;

        GLenum internal_format, format;
        switch (m_Channels) {
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
        glTextureStorage2D(m_Id, 1, internal_format, width, height);

        switch (flags & TexFlags_Filtering) {
        case TexFlags_Nearest:
                glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
        case TexFlags_Linear:
                glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }

        glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTextureSubImage2D(m_Id, 0, 0, 0, width, height, format,
                            GL_UNSIGNED_BYTE, data);

        //m_BindlessTexHandle = glGetTextureHandleARB(m_Id);
        //glMakeTextureHandleResidentARB(m_BindlessTexHandle);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
        //glMakeTextureHandleNonResidentARB(m_BindlessTexHandle);
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
