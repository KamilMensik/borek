// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Assets/AssetFlags.h"
#include "Include/Engine/Assets/IAsset.h"
#include "glm/gtc/type_ptr.hpp"
#include <filesystem>
#include <fstream>

#include "Include/Engine/Assets/FontAsset.h"

namespace Borek {

namespace fs = std::filesystem;

FontAsset::~FontAsset()
{
        if (data)
                delete[] data;
}

void
FontAsset::Serialize(const fs::path& path)
{
        std::ofstream asset(path, std::ios::binary);
        const int channels = 3;

        asset.write(RCAST<char*>(&width), sizeof(int))
             .write(RCAST<char*>(&height), sizeof(int))
             .write(RCAST<const char*>(data),
                    sizeof(uint8_t) * width * height * channels);

        asset.write(RCAST<const char*>(&ascender_y), sizeof(double))
             .write(RCAST<const char*>(&descender_y), sizeof(double))
             .write(RCAST<const char*>(&em_size), sizeof(double))
             .write(RCAST<const char*>(&line_height), sizeof(double))
             .write(RCAST<const char*>(&underline_thickness), sizeof(double))
             .write(RCAST<const char*>(&underline_y), sizeof(double));

        const uint32_t glyph_size = glyphs.size();
        asset.write(RCAST<const char*>(&glyph_size), sizeof(uint32_t));

        for (const auto& it : glyphs) {
                const FontGlyph& glyph = it.second;
                asset.write(RCAST<const char*>(&glyph.character), sizeof(char32_t))
                     .write(RCAST<const char*>(&glyph.index), sizeof(int))
                     .write(RCAST<const char*>(glm::value_ptr(glyph.quad_plane_bounds)), sizeof(glm::vec4))
                     .write(RCAST<const char*>(glm::value_ptr(glyph.quad_atlas_bounds)), sizeof(glm::vec4))
                     .write(RCAST<const char*>(&glyph.advance), sizeof(double));
        }

        const uint32_t kerring_count = kerring.size();
        asset.write(RCAST<const char*>(&kerring_count), sizeof(uint32_t));

        for (const auto& it : kerring) {
                asset.write(RCAST<const char*>(&it.first.first), sizeof(int))
                     .write(RCAST<const char*>(&it.first.second), sizeof(int))
                     .write(RCAST<const char*>(&it.second), sizeof(double));
        }
}

void
FontAsset::Deserialize(const fs::path& path)
{
        std::ifstream asset(path, std::ios::binary);
        const int channels = 3;

        asset.read(RCAST<char*>(&width), sizeof(int))
             .read(RCAST<char*>(&height), sizeof(int));

        if (data)
                delete[] data;

        data = new uint8_t[width * height * channels];
        asset.read(RCAST<char*>(data), sizeof(uint8_t) * width * height * channels)
             .read(RCAST<char*>(&ascender_y), sizeof(double))
             .read(RCAST<char*>(&descender_y), sizeof(double))
             .read(RCAST<char*>(&em_size), sizeof(double))
             .read(RCAST<char*>(&line_height), sizeof(double))
             .read(RCAST<char*>(&underline_thickness), sizeof(double))
             .read(RCAST<char*>(&underline_y), sizeof(double));

        uint32_t glyph_size;
        asset.read(RCAST<char*>(&glyph_size), sizeof(uint32_t));
        glyphs.reserve(glyph_size);

        for (int i = 0; i < glyph_size; i++) {
                char32_t c;
                asset.read(RCAST<char*>(&c), sizeof(char32_t));

                FontGlyph& glyph = glyphs[c];
                glyph.character = c;
                asset.read(RCAST<char*>(&glyph.index), sizeof(int))
                     .read(RCAST<char*>(glm::value_ptr(glyph.quad_plane_bounds)), sizeof(glm::vec4))
                     .read(RCAST<char*>(glm::value_ptr(glyph.quad_atlas_bounds)), sizeof(glm::vec4))
                     .read(RCAST<char*>(&glyph.advance), sizeof(double));
        }

        uint32_t kerring_count = kerring.size();
        asset.read(RCAST<char*>(&kerring_count), sizeof(uint32_t));

        for (int i = 0; i < kerring_count; i++) {
                int c1, c2;
                double val;
                asset.read(RCAST<char*>(&c1), sizeof(int))
                     .read(RCAST<char*>(&c2), sizeof(int))
                     .read(RCAST<char*>(&val), sizeof(double));

                kerring[{c1, c2}] = val;
        }
}

void
FontAsset::Load()
{
        font_texture = Graphics::Texture2D::Create(width, height, data, 3,
                                                   TexFlags_Linear);
}

void
FontAsset::Unload()
{
        font_texture = nullptr;
}

uint32_t
FontAsset::GetType() const
{
        return AssetType_Font;
}

}  // namespace Borek
