// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float4.hpp>

#include "Include/Base/Entity.h"
#include "Include/Base/Scene.h"

namespace Borek {

namespace Graphics {

class Texture2D;

}  // namespace Graphics

struct CameraComponent;
struct TransformComponent;
struct SpriteComponent;
class SubSprite;
class Sprite;
class Font;

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
        static void DrawQuad(const glm::vec2& position, const glm::vec2& size,
                             const Asset<SpriteSheetAsset> spritesheet,
                             uint16_t row, uint16_t col, unsigned zindex = 0);
        static void DrawQuad(const TransformComponent& transform,
                             const SpriteComponent& sprite);
        static void DrawLine(const glm::vec2& start, const glm::vec2& end,
                             const glm::vec4& color);

        static void DrawFont(const std::string& string, const Ref<Font>& font,
                             const glm::vec2& position, float size);

        static void
        DrawQuadOutline(const TransformComponent& tc, const glm::vec4& color,
                        float thickness = 0.05f);


        static void DrawScene(Ref<Scene> scene);
        static void DrawSceneNodeBegin(Entity e);
        static void DrawSceneNodeEnd(Entity e);

        static Ref<Graphics::Texture2D> WhiteTexture();

        static TransformComponent s_GlobalTransform;
};

}  // namespace Borek
