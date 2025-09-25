// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <utility>
#include <filesystem>
#include <unordered_map>

#include <glm/ext/vector_float4.hpp>

#include "Include/Core.h"
#include "Include/Graphics/Texture.h"
#include "Include/Engine/Utils/PairHasher.h"

namespace msdf_atlas {
        class GlyphGeometry;
};
namespace Borek {

class Font {
public:
        struct Glyph {
                char32_t character;
                int index;
                glm::vec4 quad_plane_bounds;
                glm::vec4 quad_atlas_bounds;
                double advance;
        };
public:
        Font() {};
        Font(const std::filesystem::path& path);

        Ref<Graphics::Texture2D>
        GetTexture();
        
        double
        GetAdvance(char32_t current, char32_t next);

        Glyph&
        GetGlyph(char32_t character);

        double 
        GetAscenderY();

        double 
        GetDescenderY();

        double 
        GetEmSize();

        double 
        GetLineHeight();

        double 
        GetUnderlineThickness();

        double 
        GetUnderlineY();

private:
        double m_AscenderY;
        double m_DescenderY;
        double m_EmSize;
        double m_LineHeight;
        double m_UnderlineThickness;
        double m_UnderlineY;

        Ref<Graphics::Texture2D> m_FontTexture;
        std::unordered_map<char32_t, Glyph> m_Glyphs;
        std::unordered_map<std::pair<int, int>, double, PairHash> m_Kerring;

        void
        GenerateFont(const std::filesystem::path& font_path,
                     const std::filesystem::path& asset_path);

        void
        LoadFont(const std::filesystem::path& asset_path);

        void
        RegisterGlyph(const msdf_atlas::GlyphGeometry& glyph,
                      std::ofstream& asset);
};

}  // namespace Borek
