// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "glm/ext/vector_float2.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Include/Engine/FZX/LTDGrid/SmallList.h"
#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

class CGrid {
public:
        struct RayCollisionData;
        CGrid(float x1, float y1, float x2, float y2,
              uint32_t cell_size_x, uint32_t cell_size_y);

        void
        Insert(uint32_t entity_id, const SimdVec4f& rect);

        void
        Update(uint32_t entity_id, const SimdVec4f& new_rect);

        void
        Remove(uint32_t entity_id);

        // Get collisions between a rectangle and other colliders
        void
        GetCollisions(const SimdVec4f& rect, uint32_t ignore_id,
                      SmallList<uint32_t>* res);

        // Get Collisions between a point and other colliders
        void
        GetCollisions(glm::vec2 pos, uint32_t ignore_id, 
                      SmallList<uint32_t>* res);

        bool
        static RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir,
                         const SimdVec4f& target, RayCollisionData& out);

        glm::vec2
        MoveAndCollide(uint32_t entity_id, float dx, float dy, uint32_t& flags);

        bool
        Contains(uint32_t entity_id);

private:
        struct CGridNode {
                uint32_t element_id;
                uint32_t next;

                CGridNode() = default;
                CGridNode(uint32_t element_id, uint32_t next = UINT32_MAX);
        };

        struct CGridElement {
                SimdVec4f rect;
                uint32_t entity_id;

                CGridElement() = default;
                CGridElement(const SimdVec4f& rect, uint32_t entity_id);
        };

        float x, y;
        uint32_t rows_count, cols_count;
        float inv_cell_size_x, inv_cell_size_y;

        std::vector<uint32_t> cells;
        FreeList<CGridNode> nodes;
        FreeList<CGridElement> elements;
        std::unordered_map<uint32_t, uint32_t> entity_element_map;

        SimdVec4f xy_vec;  // Used for SIMD vector calculations
        SimdVec4f inv_cell_size_vec;  // Used for SIMD vector calculations
        SimdVec4i rc_count_vec;  // Used for SIMD clamping

        uint32_t GetCellIndex(uint32_t col, uint32_t row);
        SimdVec4i GetPositionInGrid(const SimdVec4f& rect);

        void AddCellNode(uint32_t cell_id, uint32_t element_id);
        void DeleteCellNode(uint32_t cell_id, uint32_t element_id);
};

struct CGrid::RayCollisionData {
        glm::vec2 contact_point;
        glm::vec2 contact_normal;
        float t_hit_near;
};

}  // namespace FZX
}  // namespace Borek
