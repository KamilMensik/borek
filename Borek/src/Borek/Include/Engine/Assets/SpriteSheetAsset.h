// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "glm/ext/vector_float4.hpp"
#include <cstdint>
#include <filesystem>

#include "Include/Core.h"
#include "Include/Engine/Assets/IAsset.h"
#include "Include/Graphics/Texture.h"

namespace Borek {

struct SpriteSheetAsset : public IAsset {
        ~SpriteSheetAsset() override;

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

        uint32_t
        GetRows() const;

        uint32_t
        GetCols() const;

        uint32_t
        GetCells() const;

        inline glm::vec4
        SubTextureCords(uint32_t index) const
        {
                return SubTextureCords(index / GetCols(), index % GetCols());
        }

        glm::vec4
        SubTextureCords(uint32_t row, uint32_t col) const;

        operator Ref<Graphics::Texture2D>();

        Ref<Graphics::Texture2D> texture;
        uint8_t* data = nullptr;
        int width, height, channels;
        uint32_t step_x = 16, step_y = 16, gap_x = 0, gap_y = 0;
        uint32_t flags = 0;
};


}  // namespace Borek

