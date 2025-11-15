// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <vector>

#include "Include/Core.h"
#include "Include/Graphics/IndexBuffer.h"
#include "Include/Graphics/VertexArray.h"
#include "Include/Graphics/VertexBuffer.h"
#include "Include/Components/CameraComponent.h"
#include "Include/Components/TransformComponent.h"
#include "Include/Graphics/Shader.h"

namespace Borek {
namespace Drawing {

struct BRendererData {
        BRendererData() = default;
        BRendererData(size_t elem_size, uint32_t index_count,
                      uint32_t batch_size);
        ~BRendererData();

        // Please call this after adding an element
        void ElemAddedCallback();

        uint32_t batch_size;
        uint32_t elem_size;
        uint8_t* data = nullptr;
        uint32_t* index_data = nullptr;
        void (*draw_callback)(BRendererData& data);
        uint32_t data_count = 0;
        uint32_t index_data_count = 0;

        Ref<Graphics::VertexBuffer> vertex_buf;
        Ref<Graphics::IndexBuffer> index_buf;
        Ref<Graphics::VertexArray> vertex_arr;
        Ref<Graphics::Shader> shader;
};

class BatchRenderer {
public:
        template <class T>
        static uint32_t
        RegisterBatchType(uint32_t batch_size, uint32_t index_count,
                          void (*draw_callback)(BRendererData& data) = DrawFuncDefault)
        {
                static uint32_t index = UINT32_MAX;
                if (index != UINT32_MAX)
                        return index;

                index = s_RenderData.size();
                s_RenderData.emplace_back(sizeof(T), index_count, batch_size);
                s_RenderData.back().draw_callback = draw_callback;
                return index;
        }
        
        static inline BRendererData&
        GetBatch(uint32_t index) {
                return s_RenderData[index];
        }

        static void
        Draw(BRendererData& rdata);

        inline static void
        Draw(uint32_t index)
        {
                Draw(s_RenderData[index]);
        }

        static void
        DrawAll();

        static void
        Init();

        static void
        Deinitialize();

        static void
        Begin(CameraComponent& cam, TransformComponent& transform);

        static void
        End();

private:
        static void
        DrawFuncDefault(BRendererData& rdata);

        static std::vector<BRendererData> s_RenderData;
};

}  // namespace Drawing
}  // namespace Borek
