// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

namespace Borek {
namespace Graphics {

class Shader {
public:
        static Shader* Create(const std::string& vertex_source,
                              const std::string& fragment_source);
        virtual ~Shader() {}
        virtual void Bind() = 0;
};

}  // namespace Graphics
}  // namespace Borek
