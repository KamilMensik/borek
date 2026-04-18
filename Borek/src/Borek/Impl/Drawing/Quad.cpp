// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>

#include <format>
#include <glm/ext/vector_float2.hpp>

#include "Include/Base/Colors.h"
#include "Include/Graphics/Texture.h"
#include "Include/Components/SpriteComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Drawing/DrawingGlobals.h"
#include "Include/Drawing/Quad.h"
#include "Include/Drawing/BatchRenderer.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Platform/SIMD.h"

namespace Borek {
namespace Drawing {

constexpr static const int texture_uniforms[32] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

constexpr static const glm::vec2 tex_cords[4] {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
};

constexpr static const glm::vec2 tex_cords_i[4] {
        { 0, 0 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
};

static inline const Ref<Graphics::Texture2D>&
get_tex_safe(const SpriteComponent& sprite)
{
        if (sprite.texture.IsValid())
                return sprite.texture->texture;

        return Globals::GetData().white_tex;
}

static inline const Ref<Graphics::Texture2D>&
get_tex_safe(const AnimatedSpriteComponent& sprite)
{
        if (sprite.anim.IsValid() && sprite.anim->sprite_sheet.IsValid() &&
            !sprite.current_animation.IsNil())
                return sprite.anim->sprite_sheet->texture;

        return Globals::GetData().white_tex;
}

static inline glm::vec4
get_tex_cords_safe(const AnimatedSpriteComponent& sprite)
{
        if (!(sprite.anim.IsValid() && sprite.anim->sprite_sheet.IsValid() &&
              sprite.current_frame != 0))
                return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        const uint32_t cords = sprite.anim->animation_frames[sprite.current_frame];
        return sprite.anim->sprite_sheet->SubTextureCords(cords);
}

std::string to_s(const glm::vec4& in)
{
        return std::format("[{}, {}, {}, {}]", in.x, in.y, in.z, in.w);
}

std::string to_s(const glm::vec3& in)
{
        return std::format("[{}, {}, {}]", in.x, in.y, in.z);
}

std::string to_s(const glm::vec2& in)
{
        return std::format("[{}, {}]", in.x, in.y);
}

void
Quad::Draw(const glm::vec2& pos, const glm::vec2& size,
           const Ref<Graphics::Texture2D>& texture,
           const ColorF& color, float zindex, const glm::vec4& tex_cords)
{
        auto& rdata = BatchRenderer::GetBatch(GetRDataIndex());
        QuadData& qdata = RCAST<QuadData*>(rdata.data)[rdata.data_count++];
        const float tex_id = GetTextureIndex(texture);

        qdata.vertexes[0] = { glm::vec3(pos, zindex), color, { tex_cords.x, tex_cords.y }, tex_id };
        qdata.vertexes[1] = { glm::vec3(pos.x + size.x, pos.y, zindex), color, { tex_cords.z, tex_cords.y }, tex_id };
        qdata.vertexes[2] = { glm::vec3(pos.x + size.x, pos.y + size.y, zindex), color, { tex_cords.z, tex_cords.w }, tex_id };
        qdata.vertexes[3] = { glm::vec3(pos.x, pos.y + size.y, zindex), color, { tex_cords.x, tex_cords.w }, tex_id };
        AddIndexes(rdata);

        rdata.ElemAddedCallback();
}

void
Quad::Draw(const glm::vec2& pos, const glm::vec2& size, float rotation,
           const Ref<Graphics::Texture2D>& texture,
           const ColorF& color, float zindex)
{
        auto& rdata = BatchRenderer::GetBatch(GetRDataIndex());
        QuadData& qdata = RCAST<QuadData*>(rdata.data)[rdata.data_count++];
        const float tex_id = GetTextureIndex(texture);

        float4 points_x;
        float4 points_y;
        simd_rotate_rect(pos, size, rotation, points_x, points_y);

        qdata.vertexes[0] = { glm::vec3(points_x[0], points_y[0], zindex), color, { 0.0f, 0.0f }, tex_id };
        qdata.vertexes[1] = { glm::vec3(points_x[1], points_y[1], zindex), color, { 1.0f, 0.0f }, tex_id };
        qdata.vertexes[2] = { glm::vec3(points_x[2], points_y[2], zindex), color, { 1.0f, 1.0f }, tex_id };
        qdata.vertexes[3] = { glm::vec3(points_x[3], points_y[3], zindex), color, { 0.0f, 1.0f }, tex_id };
        AddIndexes(rdata);

        rdata.ElemAddedCallback();
}

void
Quad::Draw(const TransformComponent& transform, const SpriteComponent& sprite,
           float zindex)
{
        auto& rdata = BatchRenderer::GetBatch(GetRDataIndex());
        QuadData& qdata = RCAST<QuadData*>(rdata.data)[rdata.data_count++];

        const float tex_id = GetTextureIndex(get_tex_safe(sprite));
        const glm::vec2& pos = transform.position;
        const glm::vec2& size = scale_tof(transform.scale) * glm::vec2(sprite.size_x,
                                                            sprite.size_y);
        const float rot = transform.rotation;
        const Color& color = sprite.color;

        int tex_increment;
        int tex_pos;

        switch (sprite.flags & (uint32_t)SpriteComponentFlags_Flipping) {
        case SpriteComponentFlags_FlipX:
                tex_pos = 1;
                tex_increment = -1;
                break;
        case SpriteComponentFlags_FlipY:
                tex_pos = 3;
                tex_increment = -1;
                break;
        case SpriteComponentFlags_FlipX | SpriteComponentFlags_FlipY:
                tex_pos = 2;
                tex_increment = 1;
                break;
        default:
                tex_pos = 0;
                tex_increment = 1;
                break;
        }

        float4 points_x;
        float4 points_y;
        simd_rotate_rect(pos, size, rot, points_x, points_y);

        for (unsigned i = 0; i < 4; i++) {
                const glm::vec3 vpos = { points_x[i], points_y[i], zindex };

                qdata.vertexes[i] = { vpos, color, tex_cords[tex_pos], tex_id };

                tex_pos = (tex_pos + tex_increment) & 3;
        }

        AddIndexes(rdata);

        rdata.ElemAddedCallback();
}

void
Quad::Draw(const TransformComponent& transform,
           const AnimatedSpriteComponent& sprite,
           float zindex)
{
        auto& rdata = BatchRenderer::GetBatch(GetRDataIndex());
        QuadData& qdata = RCAST<QuadData*>(rdata.data)[rdata.data_count++];

        const float tex_id = GetTextureIndex(get_tex_safe(sprite));
        const glm::vec2& pos = transform.position;
        const glm::vec2& size = scale_tof(transform.scale) * glm::vec2(sprite.size_x,
                                                            sprite.size_y);
        const Color& color = sprite.color;

        int tex_increment;
        int tex_pos;

        switch (sprite.flags & AnimatedSpriteComponentFlags_Flipping) {
        case AnimatedSpriteComponentFlags_FlipX:
                tex_pos = 1;
                tex_increment = -1;
                break;
        case AnimatedSpriteComponentFlags_FlipY:
                tex_pos = 3;
                tex_increment = -1;
                break;
        case AnimatedSpriteComponentFlags_FlipX | AnimatedSpriteComponentFlags_FlipY:
                tex_pos = 2;
                tex_increment = 1;
                break;
        default:
                tex_pos = 0;
                tex_increment = 1;
                break;
        }

        const glm::vec4 bounds = get_tex_cords_safe(sprite);

        for (unsigned i = 0; i < 4; i++) {
                const glm::vec2 pos_increase = {
                        (i == 1 || i == 2) ? size.x : 0,
                        i >= 2 ? size.y : 0
                };
                const glm::vec2& bindex = tex_cords_i[tex_pos];
                const glm::vec2 tcord(bindex.x ? bounds.z : bounds.x,
                                      bindex.y ? bounds.w : bounds.y);

                qdata.vertexes[i] = { glm::vec3(pos + pos_increase, zindex),
                                      color, tcord, tex_id };

                tex_pos = (tex_pos + tex_increment) & 3;
        }

        AddIndexes(rdata);

        rdata.ElemAddedCallback();
}

void
Quad::Draw(const std::vector<glm::vec2>& vertices,
     const Color& color, float zindex)
{
        auto& rdata = BatchRenderer::GetBatch(GetRDataIndex());
        float tex_id = GetTextureIndex(Globals::GetData().white_tex);

        for (unsigned i = 0; i < vertices.size() / 4; i++) {
                QuadData& qdata = RCAST<QuadData*>(rdata.data)[rdata.data_count++];
                for (unsigned j = 0; j < 4; j++) {
                        const glm::vec3 vpos = { vertices[i * 4 + j], zindex };

                        qdata.vertexes[j] = { vpos, color, tex_cords[j], tex_id };
                }

                AddIndexes(rdata);
        }
}

void
Quad::Init()
{
        s_Textures.reserve(32);

        s_RdataIndex = BatchRenderer::RegisterBatchType<QuadData>(20000, 6, DrawFunc);
        BRendererData& rdata = BatchRenderer::GetBatch(s_RdataIndex);
        rdata.shader = Globals::GetData().quad_shader;
        rdata.vertex_buf->SetBufferDescriptor({
                { Datatype::Float3, "a_Position"},
                { Datatype::Float4, "a_Color" },
                { Datatype::Float2, "a_TexCord" },
                { Datatype::Float, "a_TextureId" },
        });
}

void
Quad::AddIndexes(BRendererData& rdata)
{
        const uint32_t rdata_count = rdata.data_count * 4;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 4;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 3;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 2;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 2;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 1;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 4;
}

uint32_t
Quad::GetTextureIndex(const Ref<Graphics::Texture2D> tex)
{
        if (auto it = s_TextureMap.find(tex->GetId()); it != s_TextureMap.end())
                return it->second;

        if (s_Textures.size() == 32) {
                BatchRenderer::Draw(s_RdataIndex);
        }

        const uint32_t index = s_Textures.size();
        s_TextureMap[tex->GetId()] = index;
        s_Textures.emplace_back(tex);

        return index;
}

void
Quad::DrawFunc(BRendererData& rdata)
{
        for (int i = 0; auto& tex : s_Textures) {
                tex->Bind(i);
                i++;
        }
        rdata.shader->SetUniform("u_Textures", texture_uniforms, 32);

        Graphics::Renderer::Add(*rdata.vertex_arr);
        s_TextureMap.clear();
        s_Textures.clear();
}

uint32_t Quad::s_RdataIndex = UINT32_MAX;
std::unordered_map<uint32_t, uint32_t> Quad::s_TextureMap;
std::vector<Ref<Graphics::Texture2D>> Quad::s_Textures;

}  // namespace Drawing
}  // namespace Borek
