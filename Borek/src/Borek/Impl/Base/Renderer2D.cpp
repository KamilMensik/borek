// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Colors.h"
#include "Include/Base/Components.h"
#include "Include/Debug/Log.h"
#include "Include/Graphics/IndexBuffer.h"
#include "glm/ext/matrix_float4x4.hpp"
#include <array>
#include <cstdint>
#include <string>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Include/Core.h"
#include "Include/Base/Renderer2D.h"
#include "Include/Graphics/Renderer.h"
#include "Include/Graphics/Shader.h"
#include "Include/Graphics/VertexArray.h"
#include "Include/Graphics/VertexBuffer.h"
#include "Include/Base/Scene.h"

#define _BOREK_BATCH_RENDER_SIZE 40000
#define _BOREK_MAX_TEXTURES_COUNT 32

namespace Borek {

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
                   const glm::vec2& tex_cord, float tex,
                   uint32_t entity_id = UINT32_MAX) : pos(pos), color(color),
                                                      tex_cord(tex_cord),
                                                      tex_id(tex)        {
        }

        QuadVertex() {}
};

static struct {
        Ref<Graphics::VertexArray> quad_vertex_array;
        Ref<Graphics::Shader> shader;
        Ref<Graphics::Texture2D> white_texture;
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

        static void AddQuadIndexes() {
                s_Indexes[s_IndexesCount++] = s_VertexCount - 4;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 3;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 2;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 2;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 1;
                s_Indexes[s_IndexesCount++] = s_VertexCount - 4;
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

                s_Textures[0] = data.white_texture;
                s_CurrentTextureCount = 1;

                data.shader->SetUniform("u_Textures", s_TextureUniforms, 32);
        }

        static void Shutdown()
        {
                s_VertexBuffer = nullptr;
                s_IndexBuffer = nullptr;
                s_VertexArray = nullptr;
                s_Textures.fill(nullptr);
        }

        static void Flush()
        {
                s_VertexBuffer->SetData(s_VertexData, sizeof(QuadVertex) * s_VertexCount);
                s_IndexBuffer->SetData(s_Indexes, s_IndexesCount);

                for (unsigned i = 0; i < s_CurrentTextureCount; i++) {
                        s_Textures[i]->Bind(i);
                }

                Graphics::Renderer::SetShader(data.shader);
                Graphics::Renderer::Add(*s_VertexArray);

                s_VertexCount = 0;
                s_IndexesCount = 0;
                s_CurrentTextureCount = 1;
                s_TexturesMap.clear();
        }

private:
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


void Renderer2D::Init()
{
        using namespace Graphics;

        static const uint8_t white_texture[] = { 0xff, 0xff, 0xff, 0xff };

        data.shader = Shader::Create(ASSET_PATH("shaders/2DBaseShader.glsl"));
        data.shader->Bind();

        data.white_texture = Texture2D::Create(1, 1, white_texture);
        data.white_texture->Bind(0);

        BatchRenderer::Init();
}

void Renderer2D::Shutdown()
{
        data.shader = nullptr;
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
        glm::mat4 trans = transform.GetTransformMat(s_GlobalPos);

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

void Renderer2D::DrawScene(Ref<Scene> scene)
{
        DrawSceneNode(*scene->m_RootNode);
}

void Renderer2D::DrawSceneNode(Node& node)
{
        Entity& e = node.entity;
        TransformComponent& trans = e.GetComponent<TransformComponent>();
        s_GlobalPos += trans.position;

        if (node.entity.HasComponent<SpriteComponent>()) {
                DrawQuad(trans,
                         e.GetComponent<SpriteComponent>());
        }

        if (node.first_child) {
                node.EachChildReverse([](Node* child){
                        DrawSceneNode(*child);
                });
        }

        s_GlobalPos -= trans.position;
}

Ref<Graphics::Texture2D> Renderer2D::WhiteTexture()
{
        return data.white_texture;
}

glm::vec2 Renderer2D::s_GlobalPos = glm::vec2(0.0f);

}  // namespace Borek
