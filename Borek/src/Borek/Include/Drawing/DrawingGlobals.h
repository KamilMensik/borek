// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Core.h"
#include "Include/Graphics/Shader.h"
#include "Include/Graphics/Texture.h"

namespace Borek {
namespace Drawing {

class Globals {
public:
        struct Data {
                Ref<Graphics::Texture2D> white_tex;
                Ref<Graphics::Shader> quad_shader;
                Ref<Graphics::Shader> font_shader;
        };

        static void
        Init();

        static void
        Deinitialize();

        inline static const Data&
        GetData()
        {
                return s_Data;
        }

private:
        static Data s_Data;
};

}  // namespace Drawing
}  // namespace Borek
