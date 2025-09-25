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
        OpenGLShader(const std::string& path);
        ~OpenGLShader() override;
        void Bind() const override;
        const std::string& GetName() const override { return m_Name; }
        uint32_t GetId() const override { return m_Id; };

        void SetUniform(const std::string name,
                        const glm::mat4& mat) override;
        void SetUniform(const std::string name,
                        const glm::vec4& vec) override;
        void SetUniform(const std::string name,
                        const int num) override;
        void SetUniform(const std::string name,
                        const int* nums, uint32_t count) override;

private:
        std::string m_Name;
        uint32_t m_Id;

        bool Compile(const std::string& vertex_source,
                     const std::string& fragment_source);
        bool Compile(const std::string& path);
        void LoadShaderOptions(std::ifstream& file);
};

}  // namespace Graphics
}  // namespace Borek
