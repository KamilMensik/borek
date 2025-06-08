// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <string>
#include <unordered_map>

#include "Include/Graphics/Shader.h"

#pragma once

namespace Borek {
namespace Graphics {

class ShaderLibrary {
public:
        inline static Ref<Shader> Add(const std::string& filepath)
        {
                Ref<Shader> shader = Shader::Create(filepath);
                s_Data[shader->GetName()] = shader;
                return shader;
        }

        inline static Ref<Shader> Get(const std::string& name)
        {
                if (auto s = s_Data.find(name); s != s_Data.end())
                        return s->second;

                return nullptr;
        }

        static void Remove(const std::string& name)
        {
                s_Data.erase(name);
        }

private:
        static std::unordered_map<std::string, Ref<Shader>> s_Data;
};

}  // namespace Graphics
}  // namespace Borek
