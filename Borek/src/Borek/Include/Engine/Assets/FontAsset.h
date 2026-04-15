// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Utils/PairHasher.h"
#include "Include/Graphics/Texture.h"
#include <cstdint>
#include <filesystem>
#include <string>

#include <glm/vec4.hpp>

#include "Include/Engine/Assets/IAsset.h"
#include "Include/Core.h"

namespace Borek {

struct FontGlyph {
        char32_t character;
        int index;
        glm::vec4 quad_plane_bounds;
        glm::vec4 quad_atlas_bounds;
        double advance;
};

struct FontAsset : public IAsset {
        ~FontAsset() override;

        void
        Serialize(const std::filesystem::path &path) override;

        void
        Deserialize(const std::filesystem::path &path) override;

        void
        Load() override;

        void
        Unload() override;

        uint32_t
        GetType() const override;

        Ref<Graphics::Texture2D> font_texture;
        std::unordered_map<char32_t, FontGlyph> glyphs;
        std::unordered_map<std::pair<int, int>, double, PairHash> kerring;
        uint8_t* data = nullptr;
        double ascender_y;
        double descender_y;
        double em_size;
        double line_height;
        double underline_thickness;
        double underline_y;
        int width, height;
};


}  // namespace Borek
