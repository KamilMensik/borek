// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Graphics/Shader.h"

namespace Borek {
namespace Graphics {

class OpenGLShader : public Shader {
public:
        OpenGLShader(const std::string& vertex_source,
                     const std::string& fragment_source);
        ~OpenGLShader() override;
        void Bind() override;

private:
        uint32_t m_Id;

        virtual bool Compile(const std::string& vertex_source,
                             const std::string& fragment_source);
};

}  // namespace Graphics
}  // namespace Borek
