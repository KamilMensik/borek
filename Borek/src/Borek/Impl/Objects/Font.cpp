// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "ext/save-png.h"
#include "glm/gtc/type_ptr.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>

#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "Include/Objects/Font.h"
#include "Include/Graphics/Texture.h"
#include "Include/Debug/Log.h"
#include "Include/Debug/Assert.h"

namespace fs = std::filesystem;

namespace Borek {

/*
 *      Font asset layout
 *
 *      // Font atlas
 *      4B - Texture width
 *      4B - Texture height
 *      4B - Texture channels
 *      nB - Texture
 *
 *      // Font metrics
 *      8B - ascenderY
 *      8B - descenderY
 *      8B - emSize
 *      8B - lineHeight
 *      8B - underlineThickness
 *      8B - underlineY
 *      
 *      // Character data
 *      4B Character count
 *      n * {
 *              4B Character (in char32_t)
 *              4B Character index [used for kerning]
 *              16B QuadPlaneBounds (l, r, t, b)
 *              16B QuadAtlasBounds (l, r, t, b)
 *              8B Advance
 *      }
 *
 *      // Kerning
 *      4B Kerning size
 *      n * {
 *              4B index1
 *              4B index2
 *              8B Kerning advance
 *      }
 *
 * */
Font::Font(const std::filesystem::path& path)
{

        fs::path asset_path = path;
        asset_path.replace_extension("fnt");

        if (fs::exists(asset_path)) {
                LoadFont(asset_path);
        } else {
                GenerateFont(path, asset_path);
        }

}

Ref<Graphics::Texture2D>
Font::GetTexture()
{
        return m_FontTexture;
}

double
Font::GetAdvance(char32_t current, char32_t next)
{
        Glyph& g1 = GetGlyph(current);
        Glyph& g2 = GetGlyph(next);

        double advance = g1.advance;
        if (auto res = m_Kerring.find(std::pair(g1.index, g2.index)); res != m_Kerring.end()) {
                advance = res->second;
        }

        return advance;
}

Font::Glyph&
Font::GetGlyph(char32_t character)
{
        if (auto res = m_Glyphs.find(character); res != m_Glyphs.end()) {
                return res->second;
        }

        return GetGlyph('?');
}

double 
Font::GetAscenderY()
{
        return m_AscenderY;
}

double 
Font::GetDescenderY()
{
        return m_DescenderY;
}

double 
Font::GetEmSize()
{
        return m_EmSize;
}

double 
Font::GetLineHeight()
{
        return m_LineHeight;
}

double 
Font::GetUnderlineThickness()
{
        return m_UnderlineThickness;
}

double 
Font::GetUnderlineY()
{
        return m_UnderlineY;
}

void
Font::GenerateFont(const std::filesystem::path& font_path,
                   const std::filesystem::path& asset_path)
{
        const double max_corner_angle = 3.0;

        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        BOREK_ENGINE_ASSERT(ft, "Could not initialize freetype");
        msdfgen::FontHandle* font = msdfgen::loadFont(ft, font_path.c_str());
        BOREK_ENGINE_ASSERT(font, "Could not load font");

        std::vector<msdf_atlas::GlyphGeometry> glyphs;
        msdf_atlas::FontGeometry font_geometry(&glyphs);
        font_geometry.getMetrics();
        font_geometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);

        for (auto& glyph : glyphs) {
                glyph.edgeColoring(&msdfgen::edgeColoringInkTrap,
                                   max_corner_angle, 0);
        }

        msdf_atlas::TightAtlasPacker packer;
        packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
        packer.setMinimumScale(24.0);
        packer.setScale(40.0f);
        packer.setPixelRange(2.0);
        packer.setMiterLimit(1.0);
        packer.pack(glyphs.data(), glyphs.size());
        int width = 0, height = 0;
        packer.getDimensions(width, height);
        msdf_atlas::ImmediateAtlasGenerator<
                float,
                3,
                msdf_atlas::msdfGenerator,
                msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>
        > generator(width, height);

        msdf_atlas::GeneratorAttributes attributes;
        generator.setAttributes(attributes);
        generator.setThreadCount(4);
        generator.generate(glyphs.data(), glyphs.size());

        // Convert to borek asset
        msdfgen::BitmapConstRef<uint8_t, 3> bitmap = generator.atlasStorage();
        std::ofstream asset(asset_path, std::ios::binary);
        const int channels = 3;

        asset.write(RCAST<char*>(&bitmap.width), sizeof(int))
             .write(RCAST<char*>(&bitmap.height), sizeof(int))
             .write(RCAST<const char*>(&channels), sizeof(int))
             .write(RCAST<const char*>(bitmap.pixels),
                    sizeof(uint8_t) * bitmap.width * bitmap.height * channels);

        auto& metrics = font_geometry.getMetrics();
        asset.write(RCAST<const char*>(&metrics.ascenderY), sizeof(double))
             .write(RCAST<const char*>(&metrics.descenderY), sizeof(double))
             .write(RCAST<const char*>(&metrics.emSize), sizeof(double))
             .write(RCAST<const char*>(&metrics.lineHeight), sizeof(double))
             .write(RCAST<const char*>(&metrics.underlineThickness), sizeof(double))
             .write(RCAST<const char*>(&metrics.underlineY), sizeof(double));

        uint32_t glyph_size = font_geometry.getGlyphs().size();
        asset.write(RCAST<char*>(&glyph_size), sizeof(uint32_t));

        for (auto& glyph : font_geometry.getGlyphs())
                RegisterGlyph(glyph, asset);

        const uint32_t kerring_count = font_geometry.getKerning().size();
        asset.write(RCAST<const char*>(&kerring_count), sizeof(uint32_t));

        for (auto kerring : font_geometry.getKerning()) {
                m_Kerring[kerring.first] = kerring.second;
                asset.write(RCAST<const char*>(&kerring.first.first), sizeof(int))
                     .write(RCAST<const char*>(&kerring.first.second), sizeof(int))
                     .write(RCAST<const char*>(&kerring.second), sizeof(double));
        }

        m_FontTexture = Graphics::Texture2D::Create(bitmap.width, bitmap.height, bitmap.pixels, 3);

        fs::path png_path = asset_path;
        png_path.replace_extension("png");
        msdfgen::savePng(generator.atlasStorage(), png_path.c_str());

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);
}

void
Font::LoadFont(const std::filesystem::path& asset_path)
{
        std::ifstream asset(asset_path, std::ios::binary);
        uint32_t width, height, channels;
        asset.read(RCAST<char*>(&width), sizeof(uint32_t))
             .read(RCAST<char*>(&height), sizeof(uint32_t))
             .read(RCAST<char*>(&channels), sizeof(uint32_t));

        uint8_t* tex = new uint8_t[width * height * channels];
        asset.read(RCAST<char*>(tex), width * height * channels);
        m_FontTexture = Graphics::Texture2D::Create(width, height, tex, 3);

        asset.read(RCAST<char*>(&m_AscenderY), sizeof(double))
             .read(RCAST<char*>(&m_DescenderY), sizeof(double))
             .read(RCAST<char*>(&m_EmSize), sizeof(double))
             .read(RCAST<char*>(&m_LineHeight), sizeof(double))
             .read(RCAST<char*>(&m_UnderlineThickness), sizeof(double))
             .read(RCAST<char*>(&m_UnderlineY), sizeof(double));

        uint32_t glyph_count;
        asset.read(RCAST<char*>(&glyph_count), sizeof(uint32_t));

        for (size_t i = 0; i < glyph_count; i++) {
                char32_t character;
                asset.read(RCAST<char*>(&character), sizeof(char32_t));

                Glyph& glyph = m_Glyphs[character];
                glyph.character = character;

                asset.read(RCAST<char*>(&glyph.index), sizeof(int))
                     .read(RCAST<char*>(glm::value_ptr(glyph.quad_plane_bounds)),
                           sizeof(float) * 4)
                     .read(RCAST<char*>(glm::value_ptr(glyph.quad_atlas_bounds)),
                           sizeof(float) * 4)
                     .read(RCAST<char*>(&glyph.advance), sizeof(double));
        }

        uint32_t kerring_count;
        asset.read(RCAST<char*>(&kerring_count), sizeof(uint32_t));

        for (size_t i = 0; i < kerring_count; i++) {
                std::pair<int, int> key;
                double val;

                asset.read(RCAST<char*>(&key.first), sizeof(int))
                     .read(RCAST<char*>(&key.second), sizeof(int))
                     .read(RCAST<char*>(&val), sizeof(double));

                m_Kerring[key] = val;
        }
}

void
Font::RegisterGlyph(const msdf_atlas::GlyphGeometry& glyph,
                    std::ofstream& asset)
{
        double glyph_advance = glyph.getAdvance();
        char32_t glyph_char = glyph.getCodepoint();
        int glyph_index = glyph.getIndex();
        double ld, rd, td, bd;
        float l, r, t, b;

        Glyph& glyph_internal = m_Glyphs[glyph_char];
        glyph_internal.character = glyph_char;
        glyph_internal.advance = glyph_advance;
        glyph_internal.index = glyph_index;


        glyph.getIndex();
        glyph.getQuadPlaneBounds(ld, bd, rd, td);
        l = ld; r = rd; t = td; b = bd;

        asset.write(RCAST<char*>(&glyph_char), sizeof(char32_t))
             .write(RCAST<char*>(&glyph_index), sizeof(int))
             .write(RCAST<char*>(&l), sizeof(float))
             .write(RCAST<char*>(&r), sizeof(float))
             .write(RCAST<char*>(&t), sizeof(float))
             .write(RCAST<char*>(&b), sizeof(float));

        glyph_internal.quad_plane_bounds = glm::vec4(l, r, t, b);

        glyph.getQuadAtlasBounds(ld, bd, rd, td);
        l = ld; r = rd; t = td; b = bd;
        asset.write(RCAST<char*>(&l), sizeof(float))
             .write(RCAST<char*>(&r), sizeof(float))
             .write(RCAST<char*>(&t), sizeof(float))
             .write(RCAST<char*>(&b), sizeof(float))
             .write(RCAST<char*>(&glyph_advance), sizeof(double));

        glyph_internal.quad_atlas_bounds = glm::vec4(l, r, t, b);
}

}  // namespace Borek
