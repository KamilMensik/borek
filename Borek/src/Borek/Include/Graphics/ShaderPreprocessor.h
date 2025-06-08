// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <string>
#include <regex>

#pragma once

namespace Borek {
namespace Graphics {

struct ShaderOptions {
        std::string name;

        ShaderOptions();
};

class ShaderPreprocessor {
public:
        enum class Command {
                kVertex,
                kFragment,
                kName,
        };

        static void Preprocess(const std::string& path,
                               std::stringstream& vertex_shader,
                               std::stringstream& fragment_shader);

        static inline const ShaderOptions& GetShaderOptions()
        {
                return s_Options;
        }

private:
        static ShaderOptions s_Options;
        static int s_CurrentOutput;
        static bool HandleCommand(const std::smatch& match);
};

}  // namespace Graphics
}  // namespace Borek
