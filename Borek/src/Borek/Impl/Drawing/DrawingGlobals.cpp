// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Drawing/DrawingGlobals.h"
#include "Include/Graphics/Shader.h"

namespace Borek {
namespace Drawing {

void
Globals::Init()
{
        using namespace Graphics;

        static const uint8_t white_texture[] = { 0xff, 0xff, 0xff, 0xff };

        s_Data.quad_shader = Shader::Create(ASSET_PATH("shaders/2DBaseShader.glsl"));
        s_Data.font_shader = Shader::Create(ASSET_PATH("shaders/2DFontShader.glsl"));

        s_Data.white_tex = Texture2D::Create(1, 1, white_texture);
        s_Data.white_tex->Bind(0);
}

void
Globals::Deinitialize()
{
        s_Data.white_tex = nullptr; 
        s_Data.quad_shader = nullptr;
        s_Data.font_shader = nullptr;
}

Globals::Data Globals::s_Data;


}  // namespace Drawing
}  // namespace Borek
