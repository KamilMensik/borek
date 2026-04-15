// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Components/Text2DComponent.h"
#include <cstdint>
#include <unordered_map>

#include <glm/ext/vector_float2.hpp>

#include "Include/Base/Colors.h"
#include "Include/Graphics/Texture.h"
#include "Include/Components/AnimatedSpriteComponent.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Drawing/BatchRenderer.h"
#include "Include/Drawing/DrawingGlobals.h"

namespace Borek {
namespace Drawing {

struct FontData {
        struct {
                glm::vec3 pos;
                glm::vec4 color;
                glm::vec2 tex_cord;
        } vertexes[4];
};

struct ShitData {
        struct {
                glm::vec3 pos;
                glm::vec4 color;
                glm::vec2 tex_cord;
        } vertexes[4];
};

class Font {
public:
        static void
        Draw(const TransformComponent& transform,
             const Text2DComponent& txt, float zindex = 0);

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

        static void
        RegisterTexture(const Ref<Graphics::Texture2D> tex);

        static void
        DrawFunc(BRendererData& rdata);

        static uint32_t s_RdataIndex;
        static Ref<Graphics::Texture2D> s_CurrentTexture;
};

}  // namespace Drawing
}  // namespace Borek
