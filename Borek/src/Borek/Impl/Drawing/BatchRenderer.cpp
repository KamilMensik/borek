// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Input.h"
#include "Include/Debug/Log.h"
#include <cstdint>
#include <cstring>
#include <utility>

#include "Include/Core.h"
#include "Include/Graphics/IndexBuffer.h"
#include "Include/Graphics/VertexArray.h"
#include "Include/Graphics/VertexBuffer.h"
#include "Include/Drawing/BatchRenderer.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Drawing/DrawingGlobals.h"
#include "Include/Graphics/Renderer.h"

namespace Borek {
namespace Drawing {

static uint32_t index = 0;

BRendererData::BRendererData(size_t elem_size, uint32_t index_count,
                             uint32_t batch_size)
        : batch_size(batch_size), elem_size(elem_size), index_count(index_count)
{
        const uint32_t data_size = elem_size * batch_size;
        const uint32_t index_size = index_count * batch_size;

        id = index++;
        data = new uint8_t[elem_size * batch_size];
        index_data = new uint32_t[index_size];

        vertex_buf = Graphics::VertexBuffer::Create(data, data_size, true);
        index_buf = Graphics::IndexBuffer::Create(index_data, index_size, true);

        vertex_arr = Graphics::VertexArray::Create();
        vertex_arr->AddVertexBuffer(vertex_buf);
        vertex_arr->SetIndexBuffer(index_buf);
}

BRendererData::BRendererData(BRendererData&& other)
{
        batch_size = other.batch_size;
        elem_size = other.elem_size;
        index_count = other.index_count;
        data_count = other.data_count;
        index_data_count = other.index_data_count;
        shader = std::exchange(other.shader, nullptr);
        draw_callback = other.draw_callback;
        data = std::exchange(other.data, nullptr);
        index_data = std::exchange(other.index_data, nullptr);
        vertex_buf = std::exchange(other.vertex_buf, nullptr);
        index_buf = std::exchange(other.index_buf, nullptr);
        vertex_arr = std::exchange(other.vertex_arr, nullptr);
        id = other.id;
}

BRendererData::~BRendererData()
{
        if (data)
                delete[] data;
        if (index_data)
                delete[] index_data;
}

struct BRendererData&
BRendererData::operator =(BRendererData&& other)
{
        if (data)
                delete[] data;
        if (index_data)
                delete[] index_data;

        batch_size = other.batch_size;
        elem_size = other.elem_size;
        index_count = other.index_count;
        data_count = other.data_count;
        index_data_count = other.index_data_count;
        shader = std::exchange(other.shader, nullptr);
        draw_callback = other.draw_callback;
        data = std::exchange(other.data, nullptr);
        index_data = std::exchange(other.index_data, nullptr);
        vertex_buf = std::exchange(other.vertex_buf, nullptr);
        index_buf = std::exchange(other.index_buf, nullptr);
        vertex_arr = std::exchange(other.vertex_arr, nullptr);
        id = other.id;

        return *this;
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
