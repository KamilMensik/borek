// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/FZX/Body.h"
#include "glm/trigonometric.hpp"
#include <array>
#include <cmath>
#include <cstdint>
#include <string>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include "Include/Core.h"
#include "Include/Base/Renderer2D.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Graphics/Shader.h"
#include "Include/Graphics/VertexArray.h"
#include "Include/Graphics/VertexBuffer.h"
#include "Include/Base/Scene.h"
#include "Include/Base/Application.h"
#include "Include/Base/Colors.h"
#include "Include/Base/Components.h"
#include "Include/Debug/Log.h"
#include "Include/Graphics/IndexBuffer.h"

#define _BOREK_BATCH_RENDER_SIZE 40000
#define _BOREK_MAX_TEXTURES_COUNT 32

namespace Borek {

static constexpr uint8_t ol_index(uint8_t index)
{
        return 8 - index % 8;
}

const static glm::vec4 quad_vertex_positions[4] = {
        { -0.5f, -0.5f, 0.0f, 1.0f },
        { 0.5f, -0.5f, 0.0f, 1.0f },
        { 0.5f, 0.5f, 0.0f, 1.0f },
        { -0.5f, 0.5f, 0.0f, 1.0f }
};

const static glm::vec2 quad_tex_cords[4] = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f }
};

struct QuadVertex {
        glm::vec3 pos;
        glm::vec4 color;
        glm::vec2 tex_cord;
        float tex_id;

        QuadVertex(const glm::vec3& pos, const glm::vec4& color,
                   const glm::vec2& tex_cord, float tex)
                : pos(pos), color(color), tex_cord(tex_cord), tex_id(tex) {}

        QuadVertex() {}
};

struct FontVertex {
        glm::vec3 pos;
        glm::vec4 color;
        glm::vec2 tex_cord;

        FontVertex(const glm::vec3& pos, const glm::vec4& color,
                   const glm::vec2& tex_cord)
                : pos(pos), color(color), tex_cord(tex_cord) {}

        FontVertex() {}
};

static struct {
        Ref<Graphics::VertexArray> quad_vertex_array;
        Ref<Graphics::VertexArray> font_vertex_array;
        Ref<Graphics::Texture2D> white_texture;

        Ref<Graphics::Shader> shader;
        Ref<Graphics::Shader> font_shader;
} data;

class BatchRenderer {
public:
        static void Add(const QuadVertex& vertex)
        {
                s_VertexData[s_VertexCount] = vertex;
                s_VertexCount++;
                if (s_VertexCount >= _BOREK_BATCH_RENDER_SIZE)
                        Flush();
        }

        static void Add(const FontVertex& vertex)
        {
                s_FontVertexData[s_FontVertexCount] = vertex;
                s_FontVertexCount++;
                if (s_FontVertexCount >= _BOREK_BATCH_RENDER_SIZE)
                        FlushFont();
        }

        static void AddQuadIndexes() {
                s_Indexes[s_IndexesCount++] = s_VertexCount - 4;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 3;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 2;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 2;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 1;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 4;
        }

        static void AddQuadOutlineIndexes() {
                for (int i = 0; i < 4; i++) {
                        s_Indexes[s_IndexesCount++] = s_VertexCount - ol_index(0 + i * 2);
                        s_Indexes[s_IndexesCount++] = s_VertexCount - ol_index(2 + i * 2);
                        s_Indexes[s_IndexesCount++] = s_VertexCount - ol_index(3 + i * 2);
                        s_Indexes[s_IndexesCount++] = s_VertexCount - ol_index(0 + i * 2);
                        s_Indexes[s_IndexesCount++] = s_VertexCount - ol_index(1 + i * 2);
                        s_Indexes[s_IndexesCount++] = s_VertexCount - ol_index(3 + i * 2);
                }
        }

        static void AddFontIndexes() {
                s_FontIndexes[s_FontIndexesCount++] = s_FontVertexCount - 4;
                s_FontIndexes[s_FontIndexesCount++] = s_FontVertexCount - 3;
                s_FontIndexes[s_FontIndexesCount++] = s_FontVertexCount - 2;
                s_FontIndexes[s_FontIndexesCount++] = s_FontVertexCount - 2;
                s_FontIndexes[s_FontIndexesCount++] = s_FontVertexCount - 1;
                s_FontIndexes[s_FontIndexesCount++] = s_FontVertexCount - 4;
        }

        static uint32_t Add(const Ref<Graphics::Texture> texture)
        {
                auto f = s_TexturesMap.find(texture->GetId());
                if (f == s_TexturesMap.end()) {
                        if (s_CurrentTextureCount == 32)
                                Flush();

                        s_TexturesMap[texture->GetId()] = s_CurrentTextureCount;
                        s_Textures[s_CurrentTextureCount] = texture;
                        return s_CurrentTextureCount++;
                }

                return f->second;
        }

        static void Add(const Ref<Font> font)
        {
                if (s_CurrentFont == font)
                        return;

                s_CurrentFont = font;
                //data.font_shader->SetUniform("u_FontTexture", s_CurrentFont->GetTexture()->GetId());
        }

        static void Init()
        {
                s_VertexBuffer = Graphics::VertexBuffer::Create(
                        s_VertexData, sizeof(s_VertexData), true);

                s_VertexBuffer->SetBufferDescriptor({
                        { Datatype::Float3, "a_Position"},
                        { Datatype::Float4, "a_Color" },
                        { Datatype::Float2, "a_TexCord" },
                        { Datatype::Float, "a_TextureId" },
                });

                s_IndexBuffer = Graphics::IndexBuffer::Create(s_Indexes, _BOREK_BATCH_RENDER_SIZE * 6, true);

                s_VertexArray = Graphics::VertexArray::Create();
                s_VertexArray->AddVertexBuffer(s_VertexBuffer);
                s_VertexArray->SetIndexBuffer(s_IndexBuffer);

                s_VertexCount = 0;
                s_IndexesCount = 0;

                s_FontVertexBuffer = Graphics::VertexBuffer::Create(
                        s_FontVertexData, sizeof(s_FontVertexData), true);

                s_FontVertexBuffer->SetBufferDescriptor({
                        { Datatype::Float3, "a_Position"},
                        { Datatype::Float4, "a_Color" },
                        { Datatype::Float2, "a_TexCord" },
                });

                s_FontIndexBuffer = Graphics::IndexBuffer::Create(s_FontIndexes, _BOREK_BATCH_RENDER_SIZE * 6, true);

                s_FontVertexArray = Graphics::VertexArray::Create();
                s_FontVertexArray->AddVertexBuffer(s_FontVertexBuffer);
                s_FontVertexArray->SetIndexBuffer(s_FontIndexBuffer);

                s_FontVertexCount = 0;
                s_FontIndexesCount = 0;
        }

        static void Shutdown()
        {
                s_VertexBuffer = nullptr;
                s_IndexBuffer = nullptr;
                s_VertexArray = nullptr;
                s_FontIndexBuffer = nullptr;
                s_FontVertexBuffer = nullptr;
                s_FontVertexArray = nullptr;
                s_CurrentFont = nullptr;

                s_Textures.fill(nullptr);
        }

        static void Flush()
        {
                Graphics::Renderer::SetShader(data.shader);
                data.shader->SetUniform("u_Textures", s_TextureUniforms, 32);

                s_VertexBuffer->SetData(s_VertexData, sizeof(QuadVertex) * s_VertexCount);
                s_IndexBuffer->SetData(s_Indexes, s_IndexesCount);

                for (unsigned i = 0; i < s_CurrentTextureCount; i++) {
                        s_Textures[i]->Bind(i);
                }

                Graphics::Renderer::Add(*s_VertexArray);

                s_VertexCount = 0;
                s_IndexesCount = 0;
                s_CurrentTextureCount = 1;
                s_TexturesMap.clear();

                s_Textures[0] = data.white_texture;
        }

        static void FlushFont()
        {
                if (s_FontVertexCount <= 0)
                        return;


                Graphics::Renderer::SetShader(data.font_shader);
                data.font_shader->SetUniform("u_FontTexture", 0);

                s_FontVertexBuffer->SetData(s_FontVertexData, sizeof(FontVertex) * s_FontVertexCount);
                s_FontIndexBuffer->SetData(s_FontIndexes, s_FontIndexesCount);

                s_CurrentFont->GetTexture()->Bind();

                Graphics::Renderer::Add(*s_FontVertexArray);

                s_FontVertexCount = 0;
                s_FontIndexesCount = 0;
        }

public:
        static QuadVertex s_VertexData[_BOREK_BATCH_RENDER_SIZE];
        static uint32_t s_Indexes[_BOREK_BATCH_RENDER_SIZE * 6];
        static uint32_t s_VertexCount;
        static uint32_t s_IndexesCount;
        static Ref<Graphics::VertexBuffer> s_VertexBuffer;
        static Ref<Graphics::IndexBuffer> s_IndexBuffer;
        static Ref<Graphics::VertexArray> s_VertexArray;
        static std::unordered_map<uint32_t, uint32_t> s_TexturesMap;
        static std::array<Ref<Graphics::Texture>, _BOREK_MAX_TEXTURES_COUNT> s_Textures;
        static uint32_t s_CurrentTextureCount;
        constexpr static const int s_TextureUniforms[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };

        static Ref<Font> s_CurrentFont;
        static FontVertex s_FontVertexData[_BOREK_BATCH_RENDER_SIZE];
        static uint32_t s_FontIndexes[_BOREK_BATCH_RENDER_SIZE * 6];
        static uint32_t s_FontVertexCount;
        static uint32_t s_FontIndexesCount;
        static Ref<Graphics::VertexBuffer> s_FontVertexBuffer;
        static Ref<Graphics::IndexBuffer> s_FontIndexBuffer;
        static Ref<Graphics::VertexArray> s_FontVertexArray;
};

QuadVertex BatchRenderer::s_VertexData[_BOREK_BATCH_RENDER_SIZE] = {};
uint32_t BatchRenderer::s_Indexes[_BOREK_BATCH_RENDER_SIZE * 6];
uint32_t BatchRenderer::s_VertexCount;
uint32_t BatchRenderer::s_IndexesCount;
Ref<Graphics::VertexBuffer> BatchRenderer::s_VertexBuffer;
Ref<Graphics::IndexBuffer> BatchRenderer::s_IndexBuffer;
Ref<Graphics::VertexArray> BatchRenderer::s_VertexArray;
std::unordered_map<uint32_t, uint32_t> BatchRenderer::s_TexturesMap;
std::array<Ref<Graphics::Texture>, _BOREK_MAX_TEXTURES_COUNT> BatchRenderer::s_Textures;
uint32_t BatchRenderer::s_CurrentTextureCount;

FontVertex BatchRenderer::s_FontVertexData[_BOREK_BATCH_RENDER_SIZE] = {};
uint32_t BatchRenderer::s_FontIndexes[_BOREK_BATCH_RENDER_SIZE * 6];
uint32_t BatchRenderer::s_FontVertexCount;
uint32_t BatchRenderer::s_FontIndexesCount;
Ref<Font> BatchRenderer::s_CurrentFont;
Ref<Graphics::VertexBuffer> BatchRenderer::s_FontVertexBuffer;
Ref<Graphics::IndexBuffer> BatchRenderer::s_FontIndexBuffer;
Ref<Graphics::VertexArray> BatchRenderer::s_FontVertexArray;


void Renderer2D::Init()
{
        using namespace Graphics;

        static const uint8_t white_texture[] = { 0xff, 0xff, 0xff, 0xff };

        data.shader = Shader::Create(ASSET_PATH("shaders/2DBaseShader.glsl"));
        data.font_shader = Shader::Create(ASSET_PATH("shaders/2DFontShader.glsl"));

        data.white_texture = Texture2D::Create(1, 1, white_texture);
        data.white_texture->Bind(0);

        BatchRenderer::Init();
}

void Renderer2D::Shutdown()
{
        data.shader = nullptr;
        data.font_shader = nullptr;
        BatchRenderer::Shutdown();
}

void Renderer2D::Begin(CameraComponent& camera,
                       TransformComponent& transform)
{
        Graphics::Renderer::Begin(camera, transform);
}

void Renderer2D::End()
{
        BatchRenderer::Flush();
        BatchRenderer::FlushFont();
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const glm::vec4& color, unsigned zindex)
{
        BatchRenderer::Add(QuadVertex{glm::vec3(position, zindex), color, { 0.0f, 0.0f }, 0});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y, zindex), color, { 1.0f, 0.0f }, 0});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y + size.y, zindex), color, { 1.0f, 1.0f }, 0});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x, position.y + size.y, zindex), color, { 0.0f, 1.0f }, 0});
        BatchRenderer::AddQuadIndexes();
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          Ref<Sprite> tex, unsigned zindex)
{
        float texture_index = BatchRenderer::Add(tex->GetTexture());
        BatchRenderer::Add(QuadVertex{glm::vec3(position, zindex), Colors::WHITE, { 0.0f, 0.0f }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y, zindex), Colors::WHITE, { 1.0f, 0.0f }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y + size.y, zindex), Colors::WHITE, { 1.0f, 1.0f }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x, position.y + size.y, zindex), Colors::WHITE, { 0.0f, 1.0f }, texture_index});
        BatchRenderer::AddQuadIndexes();
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          Ref<SubSprite> subsprite, unsigned zindex)
{
        float texture_index = BatchRenderer::Add(subsprite->GetTexture());
        glm::vec4 subtexture_cords = subsprite->GetBounds();
        BatchRenderer::Add(QuadVertex{glm::vec3(position, zindex), Colors::WHITE, { subtexture_cords.x, subtexture_cords.y }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y, zindex), Colors::WHITE, { subtexture_cords.z, subtexture_cords.y }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y + size.y, zindex), Colors::WHITE, { subtexture_cords.z, subtexture_cords.w }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x, position.y + size.y, zindex), Colors::WHITE, { subtexture_cords.x, subtexture_cords.w }, texture_index});
        BatchRenderer::AddQuadIndexes();
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          Ref<Sprite> tex, const glm::vec4& color,
                          unsigned zindex)
{
        float texture_index = BatchRenderer::Add(tex->GetTexture());
        BatchRenderer::Add(QuadVertex{glm::vec3(position, zindex), color, { 0.0f, 0.0f }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y, zindex), color, { 1.0f, 0.0f }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x + size.x, position.y + size.y, zindex), color, { 1.0f, 1.0f }, texture_index});
        BatchRenderer::Add(QuadVertex{glm::vec3(position.x, position.y + size.y, zindex), color, { 0.0f, 1.0f }, texture_index});
        BatchRenderer::AddQuadIndexes();
}

void Renderer2D::DrawQuad(const TransformComponent& transform,
                          const SpriteComponent& sprite)
{
        float texture_index = sprite.sprite == nullptr ? 0 : BatchRenderer::Add(sprite.sprite->GetTexture());
        glm::mat4 trans = transform.GetTransformMat();

        for (int i = 0; i < 4; i++) {
                BatchRenderer::Add(QuadVertex{
                        trans * quad_vertex_positions[i],
                        sprite.color,
                        quad_tex_cords[i],
                        texture_index,
                });
        }

        BatchRenderer::AddQuadIndexes();
}

void
Renderer2D::DrawLine(const glm::vec2& start, const glm::vec2& end,
                     const glm::vec4& color)
{
        BOREK_ENGINE_INFO("Drawing line, start: [{}, {}], end: [{}, {}]",
                          start.x, start.y, end.x, end.y);
        const glm::vec2 sub_vec = end - start;
        const glm::vec2 pow2_vec = glm::pow(sub_vec, glm::vec2(2, 2));

        const float distance = std::sqrt(pow2_vec.x + pow2_vec.y);
        const float angle = glm::degrees(std::atan2(sub_vec.y, sub_vec.x));

        TransformComponent tc;
        tc.position = start;
        tc.scale = glm::vec2(distance * 2, 0.05f);
        tc.rotation = angle;

        SpriteComponent sc;
        sc.color = color;

        DrawQuad(tc, sc);
}

void
Renderer2D::DrawQuadOutline(const TransformComponent &tc, const glm::vec4 &color,
                            float thickness)
{
        const float texture_index = 0;
        glm::mat4 trans = tc.GetTransformMat();

        for (unsigned i = 0; i < 4; i++) {
                BatchRenderer::Add(QuadVertex{trans * quad_vertex_positions[i], color, quad_tex_cords[i], texture_index, });

                float xoff = (i == 1 || i == 2) ? -thickness : thickness;
                float yoff = (i < 2) ? thickness : -thickness;

                BatchRenderer::Add(QuadVertex{trans * (quad_vertex_positions[i] + glm::vec4(xoff, yoff, 0, 0)), color, quad_tex_cords[i], texture_index, });
        }

        BatchRenderer::AddQuadOutlineIndexes();
}

void Renderer2D::DrawScene(Ref<Scene> scene)
{
        Application::GetScene()->TraverseScene(DrawSceneNodeBegin,
                                               DrawSceneNodeEnd);
}

void
Renderer2D::DrawSceneNodeBegin(Entity e)
{
        s_GlobalTransform += e.GetComponent<TransformComponent>();

        static Ref<Font> default_font = NewRef<Font>("/home/kamil/bakalarka/borek/Stavitel/assets/Fonts/MS Gothic.ttf");

        if (e.HasComponent<SpriteComponent>()) {
                DrawQuad(s_GlobalTransform, e.GetComponent<SpriteComponent>());
        }
        if (e.HasComponent<Text2DComponent>()) {
                Text2DComponent& font = e.GetComponent<Text2DComponent>();
                DrawFont(font.value, default_font, s_GlobalTransform.position, font.size);
        }
}

void
Renderer2D::DrawSceneNodeEnd(Entity e)
{
        s_GlobalTransform -= e.Transform();
}

void
Renderer2D::DrawFont(const std::string& string, const Ref<Font>& font,
                     const glm::vec2& position, float size)
{
        std::u32string str(string.begin(), string.end());
        BatchRenderer::Add(font);
        float x = 0;
        float fs_scale = 1.0 / (font->GetAscenderY() - font->GetDescenderY());
        float y = 0;

        float atlas_scale_x = 1.0 / font->GetTexture()->GetWidth();
        float atlas_scale_y = 1.0 / font->GetTexture()->GetHeight();

        for (int i = 0; i + 1 < string.size(); i++) {
                char32_t character = str[i];
                Font::Glyph& glyph = font->GetGlyph(character);
                glm::vec2 plane_bounds_min(glyph.quad_plane_bounds.x, glyph.quad_plane_bounds.w);
                glm::vec2 plane_bounds_max(glyph.quad_plane_bounds.y, glyph.quad_plane_bounds.z);

                plane_bounds_min *= fs_scale * size;
                plane_bounds_max *= fs_scale * size;
                plane_bounds_min += glm::vec2{x, y} + position;
                plane_bounds_max += glm::vec2{x, y} + position;

                glm::vec2 atlas_bounds_min(glyph.quad_atlas_bounds.x, glyph.quad_atlas_bounds.w);
                glm::vec2 atlas_bounds_max(glyph.quad_atlas_bounds.y, glyph.quad_atlas_bounds.z);
                atlas_bounds_min.x *= atlas_scale_x;
                atlas_bounds_min.y *= atlas_scale_y;
                atlas_bounds_max.x *= atlas_scale_x;
                atlas_bounds_max.y *= atlas_scale_y;

                x += font->GetAdvance(character, str[i+1]) * size;

                BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_min, 0), Colors::WHITE, atlas_bounds_min });
                BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_max.x, plane_bounds_min.y, 0), Colors::WHITE, { atlas_bounds_max.x, atlas_bounds_min.y }});
                BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_max, 0), Colors::WHITE, atlas_bounds_max });
                BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_min.x, plane_bounds_max.y, 0), Colors::WHITE, { atlas_bounds_min.x, atlas_bounds_max.y } });
                BatchRenderer::AddFontIndexes();
        }

        char32_t character = str.back();
        Font::Glyph& glyph = font->GetGlyph(character);
        glm::vec2 plane_bounds_min(glyph.quad_plane_bounds.x, glyph.quad_plane_bounds.w);
        glm::vec2 plane_bounds_max(glyph.quad_plane_bounds.y, glyph.quad_plane_bounds.z);

        plane_bounds_min *= fs_scale * size;
        plane_bounds_max *= fs_scale * size;
        plane_bounds_min += glm::vec2{x, y} + position;
        plane_bounds_max += glm::vec2{x, y} + position;

        glm::vec2 atlas_bounds_min(glyph.quad_atlas_bounds.x, glyph.quad_atlas_bounds.w);
        glm::vec2 atlas_bounds_max(glyph.quad_atlas_bounds.y, glyph.quad_atlas_bounds.z);
        atlas_bounds_min.x *= atlas_scale_x;
        atlas_bounds_min.y *= atlas_scale_y;
        atlas_bounds_max.x *= atlas_scale_x;
        atlas_bounds_max.y *= atlas_scale_y;

        BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_min, 0), Colors::WHITE, atlas_bounds_min });
        BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_max.x, plane_bounds_min.y, 0), Colors::WHITE, { atlas_bounds_max.x, atlas_bounds_min.y }});
        BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_max, 0), Colors::WHITE, atlas_bounds_max });
        BatchRenderer::Add(FontVertex{glm::vec3(plane_bounds_min.x, plane_bounds_max.y, 0), Colors::WHITE, { atlas_bounds_min.x, atlas_bounds_max.y } });
        BatchRenderer::AddFontIndexes();
}


Ref<Graphics::Texture2D> Renderer2D::WhiteTexture()
{
        return data.white_texture;
}

TransformComponent Renderer2D::s_GlobalTransform;

}  // namespace Borek
