// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>

#include "Include/Objects/Sprite.h"
#include "Include/Base/Components.h"
#include "Include/Objects/SpriteSheet.h"

namespace Borek {

class Renderer2D {
public:
        static void Init();
        static void Shutdown();
        static void Begin(CameraComponent& camera,
                          TransformComponent& transform);
        static void End();

        static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                             const glm::vec4& color, unsigned zindex = 0);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                             Ref<Sprite> tex, unsigned zindex = 0);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                             Ref<Sprite> tex, const glm::vec4& color,
                             unsigned zindex = 0);
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                             Ref<SubSprite> subsprite, unsigned zindex = 0);
        static void DrawQuad(const TransformComponent& transform,
                             const SpriteComponent& sprite,
                             uint32_t entity_id = UINT32_MAX);

        static Ref<Graphics::Texture2D> WhiteTexture();
};

}  // namespace Borek
