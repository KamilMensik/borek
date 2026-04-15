// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <codecvt>
#include <locale>

#include "Include/Graphics/Renderer.h"
#include "Include/Drawing/Font.h"

namespace Borek {
namespace Drawing {

static std::string to_s(const glm::vec2& in)
{
        return std::format("[{}, {}]", in.x, in.y);
}

void
Font::Draw(const TransformComponent& transform, const Text2DComponent& txt,
           float zindex)
{
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
        std::u32string str = conv.from_bytes(txt.value.Str().c_str());

        const FontAsset& fnt = txt.font;
        const float atlas_scale_x = 1.0 / fnt.font_texture->GetWidth();
        const float atlas_scale_y = 1.0 / fnt.font_texture->GetHeight();
        const float fs_scale = 1.0 / (fnt.ascender_y - fnt.descender_y);
        RegisterTexture(fnt.font_texture);

        float x = 0;
        float y = 0;

        for (int i = 0; i < str.size(); i++) {
                char32_t character = str[i];
                const FontGlyph& glyph = fnt.glyphs.find(character)->second;
                glm::vec2 plane_bounds_min(glyph.quad_plane_bounds.x, glyph.quad_plane_bounds.w);
                glm::vec2 plane_bounds_max(glyph.quad_plane_bounds.y, glyph.quad_plane_bounds.z);

                plane_bounds_min *= fs_scale * txt.size;
                plane_bounds_max *= fs_scale * txt.size;
                plane_bounds_min += glm::vec2{x, y} + transform.position;
                plane_bounds_max += glm::vec2{x, y} + transform.position;

                glm::vec2 atlas_bounds_min(glyph.quad_atlas_bounds.x, glyph.quad_atlas_bounds.w);
                glm::vec2 atlas_bounds_max(glyph.quad_atlas_bounds.y, glyph.quad_atlas_bounds.z);
                atlas_bounds_min.x *= atlas_scale_x;
                atlas_bounds_min.y *= atlas_scale_y;
                atlas_bounds_max.x *= atlas_scale_x;
                atlas_bounds_max.y *= atlas_scale_y;

                if (i != str.size() - 1) {
                        auto it = fnt.kerring.find({ character, str[i+1] });
                        if (it != fnt.kerring.end()) {
                                x += it->second * txt.size;
                        } else {
                                x += glyph.advance * txt.size;
                        }
                }

                auto& rdata = BatchRenderer::GetBatch(GetRDataIndex());
                FontData& fdata = RCAST<FontData*>(rdata.data)[rdata.data_count++];

                fdata.vertexes[0] = { glm::vec3(plane_bounds_min, zindex), txt.color, atlas_bounds_min };
                fdata.vertexes[1] = { glm::vec3(plane_bounds_max.x, plane_bounds_min.y, zindex), txt.color, { atlas_bounds_max.x, atlas_bounds_min.y } };
                fdata.vertexes[2] = { glm::vec3(plane_bounds_max, zindex), txt.color, atlas_bounds_max };
                fdata.vertexes[3] = { glm::vec3(plane_bounds_min.x, plane_bounds_max.y, zindex), txt.color, { atlas_bounds_min.x, atlas_bounds_max.y } };
                AddIndexes(rdata);

                rdata.ElemAddedCallback();
        }
}

void
Font::Init()
{
        s_RdataIndex = BatchRenderer::RegisterBatchType<FontData>(20000, 6, DrawFunc);
        BRendererData& rdata = BatchRenderer::GetBatch(s_RdataIndex);
        rdata.shader = Globals::GetData().font_shader;
        rdata.vertex_buf->SetBufferDescriptor({
                { Datatype::Float3, "a_Position"},
                { Datatype::Float4, "a_Color" },
                { Datatype::Float2, "a_TexCord" },
        });
}

void
Font::AddIndexes(BRendererData& rdata)
{
        const uint32_t rdata_count = rdata.data_count * 4;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 4;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 3;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 2;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 2;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 1;
        rdata.index_data[rdata.index_data_count++] = rdata_count - 4;
}

void
Font::RegisterTexture(const Ref<Graphics::Texture2D> tex)
{
        if (tex == s_CurrentTexture)
                return;

        if (s_CurrentTexture)
                BatchRenderer::Draw(s_RdataIndex);

        s_CurrentTexture = tex;
}

void
Font::DrawFunc(BRendererData& rdata)
{
        s_CurrentTexture->Bind();
        rdata.shader->SetUniform("u_FontTexture", 0);

        Graphics::Renderer::Add(*rdata.vertex_arr);
}

uint32_t Font::s_RdataIndex = UINT32_MAX;
Ref<Graphics::Texture2D> Font::s_CurrentTexture;

}  // namespace Drawing
}  // namespace Borek
