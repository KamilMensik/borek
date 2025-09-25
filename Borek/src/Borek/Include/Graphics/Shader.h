// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float4.hpp>

#include "Include/Core.h"

namespace Borek {
namespace Graphics {

class Shader {
public:
        static Ref<Shader> Create(const std::string& vertex_source,
                              const std::string& fragment_source);
        static Ref<Shader> Create(const std::string& path);
        virtual ~Shader() {}
        virtual void Bind() const = 0;
        virtual const std::string& GetName() const = 0;
        virtual uint32_t GetId() const = 0;

        // Uniform uploading.
        virtual void SetUniform(const std::string name,
                                const glm::mat4& mat) = 0;
        virtual void SetUniform(const std::string name,
                                const glm::vec4& vec) = 0;
        virtual void SetUniform(const std::string name,
                                const int num) = 0;
        virtual void SetUniform(const std::string name,
                                const int* nums, uint32_t count) = 0;
};

}  // namespace Graphics
}  // namespace Borek
