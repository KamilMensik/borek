// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Components/SpriteComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Drawing/BatchRenderer.h"
#include "Include/Drawing/DrawingGlobals.h"
#include <cstdint>

#include <glm/ext/vector_float2.hpp>

#include "Include/Base/Colors.h"
#include "Include/Graphics/Texture.h"
#include <unordered_map>

namespace Borek {
namespace Drawing {

struct QuadData {
        struct {
                glm::vec3 pos;
                glm::vec4 color;
                glm::vec2 tex_cord;
                float tex_id;
        } vertexes[4];
};

class Quad {
public:
        /*
         * Draw an unrotated quad.
         */
        static void
        Draw(const glm::vec2& pos, const glm::vec2& size,
             const Ref<Graphics::Texture2D>& texture,
             const Color& color, float zindex,
             const glm::vec4& tex_cords = {0, 0, 1, 1});

        inline static void
        Draw(const glm::vec2& pos, const glm::vec2& size,
             const Ref<Graphics::Texture2D>& texture, float zindex = 0)
        {
                Draw(pos, size, texture, Colors::WHITE, zindex);
        }

        inline static void
        Draw(const glm::vec2& pos, const glm::vec2& size,
             const Color& color, float zindex = 0)
        {
                Draw(pos, size, Globals::GetData().white_tex, color, zindex);
        }

        inline static void
        Draw(const glm::vec2& pos, const glm::vec2& size,
             float zindex = 0)
        {
                Draw(pos, size, Globals::GetData().white_tex, Colors::WHITE,
                     zindex);
        }

        /*
         * Draw a rotated quad.
         */
        static void
        Draw(const glm::vec2& pos, const glm::vec2& size, float rotation,
             const Ref<Graphics::Texture2D>& texture,
             const Color& color, float zindex);

        inline static void
        Draw(const glm::vec2& pos, const glm::vec2& size, float rotation,
             const Ref<Graphics::Texture2D>& texture, float zindex = 0)
        {
                Draw(pos, size, rotation, texture, Colors::WHITE, zindex);
        }

        inline static void
        Draw(const glm::vec2& pos, const glm::vec2& size, float rotation,
             const Color& color, float zindex = 0)
        {
                Draw(pos, size, rotation, Globals::GetData().white_tex, color,
                     zindex);
        }

        inline static void
        Draw(const glm::vec2& pos, const glm::vec2& size, float rotation,
             float zindex = 0)
        {
                Draw(pos, size, rotation, Globals::GetData().white_tex,
                     Colors::WHITE, zindex);
        }

        /*
         * Draw a quad from entity.
         */
        static void
        Draw(const TransformComponent& transform, const SpriteComponent& sprite,
             float zindex = 0);

private:
        static inline uint32_t GetRDataIndex()
        {
                if (s_RdataIndex != UINT32_MAX)
                        return s_RdataIndex;

                Init();
                return s_RdataIndex;
        }

        static void
        Init();

        static void
        AddIndexes(BRendererData& rdata);

        static uint32_t
        GetTextureIndex(const Ref<Graphics::Texture2D> tex);

        static void
        DrawFunc(BRendererData& rdata);

        static uint32_t s_RdataIndex;
        static std::unordered_map<uint32_t, uint32_t> s_TextureMap;
        static std::vector<Ref<Graphics::Texture2D>> s_Textures;
};

}  // namespace Drawing
}  // namespace Borek
