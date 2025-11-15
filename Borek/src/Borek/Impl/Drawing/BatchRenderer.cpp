// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Drawing/DrawingGlobals.h"
#include "Include/Graphics/Renderer.h"
#include <cstdint>

#include "Include/Core.h"
#include "Include/Graphics/IndexBuffer.h"
#include "Include/Graphics/VertexArray.h"
#include "Include/Graphics/VertexBuffer.h"
#include "Include/Drawing/BatchRenderer.h"

namespace Borek {
namespace Drawing {

BRendererData::BRendererData(size_t elem_size, uint32_t index_count,
                             uint32_t batch_size)
        : batch_size(batch_size), elem_size(elem_size)
{
        const uint32_t data_size = elem_size * batch_size;
        const uint32_t index_size = index_count * batch_size;

        data = new uint8_t[elem_size * batch_size];
        index_data = new uint32_t[index_size];

        vertex_buf = Graphics::VertexBuffer::Create(data, data_size, true);
        index_buf = Graphics::IndexBuffer::Create(index_data, index_size, true);

        vertex_arr = Graphics::VertexArray::Create();
        vertex_arr->AddVertexBuffer(vertex_buf);
        vertex_arr->SetIndexBuffer(index_buf);
}

BRendererData::~BRendererData()
{
        if (data)
                delete[] data;
        if (index_data)
                delete[] index_data;
}

void
BRendererData::ElemAddedCallback()
{
        if (data_count == batch_size)
                BatchRenderer::Draw(*this);
}

void
BatchRenderer::Draw(BRendererData& rdata)
{
        Graphics::Renderer::SetShader(rdata.shader);
        rdata.vertex_buf->SetData(rdata.data, rdata.elem_size * rdata.data_count);
        rdata.index_buf->SetData(rdata.index_data, rdata.index_data_count);
        rdata.draw_callback(rdata);
        rdata.data_count = 0;
        rdata.index_data_count = 0;
}

void
BatchRenderer::DrawAll()
{
        for (auto& rdata : s_RenderData) {
                Draw(rdata);
        }
}

void
BatchRenderer::Init()
{
        Drawing::Globals::Init();
}

void
BatchRenderer::Deinitialize()
{
        Drawing::Globals::Deinitialize();
}

void
BatchRenderer::Begin(CameraComponent& cam, TransformComponent& transform)
{
        Graphics::Renderer::Begin(cam, transform);
}

void
BatchRenderer::End()
{
        DrawAll();
        Graphics::Renderer::End();
}

void
BatchRenderer::DrawFuncDefault(BRendererData& rdata)
{
        Graphics::Renderer::Add(*rdata.vertex_arr);       
}

std::vector<BRendererData> BatchRenderer::s_RenderData;

}  // namespace Drawing
}  // namespace Borek
