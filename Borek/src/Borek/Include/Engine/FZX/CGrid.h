// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/FZX/Collision.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <glm/ext/vector_float2.hpp>

#include "Include/Engine/FZX/LTDGrid/SmallList.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

struct ElementRectData {
        uint16_t size_x, size_y;
        float rotation;
};

struct ElementTilemapData {
        int16_t row, col;
        uint32_t next;
};

struct ElementCircleData {
        uint16_t radius;
};

class CGrid {
public:
        struct Element {
                enum class Type : uint32_t {
                        Rect, Tilemap, Circle
                };

                M128 aabb;
                uint32_t entity_id;
                Type type;
                union {
                        ElementRectData rect_data;
                        ElementTilemapData tilemap_data;
                        ElementCircleData circle_data;
                };

                Element() = default;
                Element(M128 aabb, uint32_t entity_id, ElementRectData&& rd);
                Element(M128 aabb, uint32_t entity_id, ElementTilemapData&& td);
                Element(M128 aabb, uint32_t entity_id, ElementCircleData&& td);
        };

        struct RayCollisionData;
        CGrid(float x1, float y1, float x2, float y2,
              uint32_t cell_size_x, uint32_t cell_size_y);

        uint32_t
        Insert(uint32_t entity_id, Body& body, const glm::vec2& position,
               const glm::vec2& scale, float rotation = 0.0f);

        void
        Update(uint32_t entity_id, Body& body, const glm::vec2& position,
               const glm::vec2& scale, float rotation = 0.0f);

        void
        Update(uint32_t entity_id, M128 new_aabb);

        void
        Remove(uint32_t entity_id);

        void
        RemoveTilemapNode(uint32_t entity_id, uint32_t element_id);

        // Get collisions between a collider and other colliders
        void
        GetCollisions(uint32_t entity_id, SmallList<uint32_t>* res);

        // Get Collisions between a point and other colliders
        void
        GetCollisions(glm::vec2 pos, uint32_t ignore_id, 
                      SmallList<uint32_t>* res);

        void
        GetCollisions(const Element& rect, uint32_t ignore_id,
                      SmallList<uint32_t>* res);

        bool
        static RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir,
                         M128 target, RayCollisionData& out);

        std::pair<Collision, glm::vec2>
        MoveAndCollide(const Element& element, float dx, float dy, uint32_t& flags);

        bool
        Contains(uint32_t entity_id);

        M128
        GetAABB(uint32_t entity_id);

        const Element&
        GetElement(uint32_t entity_id);

        void
        Clear();

private:

        struct CGridNode {
                uint32_t element_id;
                uint32_t next;

                CGridNode() = default;
                CGridNode(uint32_t element_id, uint32_t next = UINT32_MAX);
        };

        float x, y;
        uint32_t rows_count, cols_count;
        float inv_cell_size_x, inv_cell_size_y;

        std::vector<uint32_t> cells;
        FreeList<CGridNode> nodes;
        FreeList<Element> elements;
        std::unordered_map<uint32_t, uint32_t> entity_element_map;

        M128 xy_vec;  // Used for SIMD vector calculations
        M128 inv_cell_size_vec;  // Used for SIMD vector calculations
        M128I rc_count_vec;  // Used for SIMD clamping

        uint32_t GetCellIndex(uint32_t col, uint32_t row);
        int4 GetPositionInGrid(M128 rect);

        void AddCellNode(uint32_t cell_id, uint32_t element_id);
        void DeleteCellNode(uint32_t cell_id, uint32_t element_id);
        void UpdateCellNode(uint32_t cell_id, uint32_t prev_el_id,
                            uint32_t new_el_id);
};

struct CGrid::RayCollisionData {
        glm::vec2 contact_point;
        glm::vec2 contact_normal;
        float t_hit_near;
};

}  // namespace FZX
}  // namespace Borek
