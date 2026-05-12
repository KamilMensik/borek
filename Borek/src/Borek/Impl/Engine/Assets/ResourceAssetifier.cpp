// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/FontAsset.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "ext/import-font.h"
#include "msdf-atlas-gen/BitmapAtlasStorage.h"
#include "msdf-atlas-gen/FontGeometry.h"
#include "msdf-atlas-gen/GlyphGeometry.h"
#include "msdf-atlas-gen/ImmediateAtlasGenerator.h"
#include "msdf-atlas-gen/TightAtlasPacker.h"
#include "msdf-atlas-gen/glyph-generators.h"
#include <cstdint>
#include <string>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>

#include <stb/image.h>

#include "Include/Debug/Assert.h"
#include "Include/Engine/Assets/ResourceAssetifier.h"
#include "Include/Engine/Utils/StringHelpers.h"
#include "Include/Base/Application.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/ScriptAsset.h"
#include "Include/Engine/Assets/TexAsset.h"
#include "Include/Engine/Utils/PathHelpers.h"

namespace fs = std::filesystem;

namespace Borek {

static bool
is_younger(const fs::path& f1, const fs::path& f2)
{
        return (fs::exists(f2) && (fs::last_write_time(f1) <= fs::last_write_time(f2)));
}

static std::string
get_class_name(const fs::path& path)
{
        static const std::regex reg(R"((?:module ([\w:\d]+))|(?:class ([\w:\d]+))|(end))");
        std::ifstream code(path);
        std::stringstream code_str;
        code_str << code.rdbuf();
        std::string subject = code_str.str();
        std::vector<std::string> res_stack;
        std::stringstream res;
        
        for (std::sregex_iterator it = std::sregex_iterator(subject.begin(), subject.end(), reg); it != std::sregex_iterator(); it++) {
                std::smatch match = *it;
                if (match[1] != "") {
                        res_stack.emplace_back(match[1]);
                } else if (match[2] != "") {
                        res_stack.emplace_back(match[2]);
                        break;
                } else if (match[3] != "") {
                        res_stack.pop_back();
                } else {
                        return "";
                }
        }

        for (auto& str : res_stack) {
                res << str << ":";
        }

        std::string cname = res.str();
        cname.pop_back();
        return cname;
}

static FontGlyph
create_glyph(const msdf_atlas::GlyphGeometry& glyph)
{
        FontGlyph result;

        double glyph_advance = glyph.getAdvance();
        char32_t glyph_char = glyph.getCodepoint();
        int glyph_index = glyph.getIndex();
        double ld, rd, td, bd;

        result.character = glyph_char;
        result.advance = glyph_advance;
        result.index = glyph_index;


        glyph.getIndex();
        glyph.getQuadPlaneBounds(ld, bd, rd, td);
        result.quad_plane_bounds = glm::vec4(ld, rd, td, bd);

        glyph.getQuadAtlasBounds(ld, bd, rd, td);
        result.quad_atlas_bounds = glm::vec4(ld, rd, td, bd);

        return result;
}

void
ResourceAssetifier::AssetifyFolder(const fs::path& path)
{
        if (path == "" || path.stem() == "build")
                return;

        for (auto& file : fs::directory_iterator(path)) {
                if (file.is_regular_file()) {
                        AssetifyFile(file.path());
                } else if (file.is_directory()) {
                        AssetifyFolder(file.path());
                }
        }
}

void
ResourceAssetifier::AssetifyFile(const fs::path& path)
{
        switch (HashP(path.extension())) {
        case Hash(".png"):
        case Hash(".jpg"):
        case Hash(".jpeg"):
                return AssetifyImage(path);
        case Hash(".rb"):
                return AssetifyScript(path);
        case Hash(".mp3"):
        case Hash(".wav"):
        case Hash(".flac"):
                return AssetifySound(path);
        case Hash(".ttf"):
                return AssetifyFont(path);
        }
}

void
ResourceAssetifier::AssetifyImage(const fs::path& path)
{
        fs::path asset_path = path;
        asset_path.replace_extension("sst");
        if (is_younger(path, asset_path))
                return;
        asset_path.replace_extension("tex");
        if (is_younger(path, asset_path))
                return;

        Uniq<TexAsset> tex = NewUniq<TexAsset>();
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* img = stbi_load(path.c_str(), &tex->width, &tex->height,
                                 &tex->channels, STBI_default);

        BOREK_ENGINE_ASSERT(img, "Image from path {} could not be loaded", path.c_str());

        const int data_size = tex->width * tex->height * tex->channels;
        tex->data = new uint8_t[data_size];
        std::memcpy(tex->data, img, data_size);
        stbi_image_free(img);

        tex->Serialize(asset_path);
        AssetManager::Refresh(Utils::Path::ToRelative(path).string(), std::move(tex));
}

void
ResourceAssetifier::AssetifyScript(const fs::path& path)
{
        static const std::regex reg(R"((?:module [\w:\d]+)|(?:class [\w:\d]+)|end)");

        auto& compiler = Application::GetRubyEngine().GetCompiler();

        fs::path asset_path = path;
        asset_path.replace_extension("scr");
        if (is_younger(path, asset_path))
                return;

        compiler.Compile(path, asset_path);
        const uint32_t code_size = fs::file_size(asset_path);
        std::ifstream bin(asset_path, std::ios::binary);
        
        Uniq<ScriptAsset> scr = NewUniq<ScriptAsset>();
        scr->class_name = get_class_name(path);
        scr->code_size = code_size;
        scr->code = new uint8_t[code_size];
        bin.read(RCAST<char*>(scr->code), code_size);

        scr->Serialize(asset_path);

        const fs::path rel_path = Utils::Path::ToRelative(asset_path);
        AssetManager::Refresh(rel_path.c_str(), std::move(scr));
}

void
ResourceAssetifier::AssetifySound(const fs::path& path)
{
        fs::path asset_path = path;
        asset_path.replace_extension("snd");
        if (is_younger(path, asset_path))
                return;

        Uniq<SoundAsset> snd = NewUniq<SoundAsset>();
        snd->filepath = path;
        snd->Serialize(asset_path);

        AssetManager::Refresh(Utils::Path::ToRelative(path).string(), std::move(snd));
}

void
ResourceAssetifier::AssetifyFont(const std::filesystem::path& path)
{
        const double max_corner_angle = 3.0;

        fs::path asset_path = path;
        asset_path.replace_extension("fnt");
        if (is_younger(path, asset_path))
                return;

        Uniq<FontAsset> fnt = NewUniq<FontAsset>();

        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
        BOREK_ENGINE_ASSERT(ft, "Could not initialize freetype");
        msdfgen::FontHandle* font = msdfgen::loadFont(ft, path.c_str());
        BOREK_ENGINE_ASSERT(font, "Could not load font");

        std::vector<msdf_atlas::GlyphGeometry> glyphs;
        msdf_atlas::FontGeometry font_geometry(&glyphs);
        auto& metrics = font_geometry.getMetrics();
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

        msdfgen::BitmapConstRef<uint8_t, 3> bitmap = generator.atlasStorage();

        for (auto& glyph : font_geometry.getGlyphs())
                fnt->glyphs[glyph.getIndex()] = create_glyph(glyph);

        for (auto kerring : font_geometry.getKerning())
                fnt->kerring[kerring.first] = kerring.second;

        fnt->ascender_y = metrics.ascenderY;
        fnt->descender_y = metrics.descenderY;
        fnt->em_size = metrics.emSize;
        fnt->line_height = metrics.lineHeight;
        fnt->underline_thickness = metrics.underlineThickness;
        fnt->underline_y = metrics.underlineY;

        fnt->width = bitmap.width;
        fnt->height = bitmap.height;
        const uint32_t bitmap_size = bitmap.width * bitmap.height * 3;
        fnt->data = new uint8_t[bitmap_size];
        memcpy(fnt->data, bitmap.pixels, bitmap_size);
        fnt->font_texture = Graphics::Texture2D::Create(bitmap.width, bitmap.height, bitmap.pixels, 3);

        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(ft);

        fnt->Serialize(asset_path);
        AssetManager::Refresh(Utils::Path::ToRelative(path).string(), std::move(fnt));
}

}  // namespace Borek
