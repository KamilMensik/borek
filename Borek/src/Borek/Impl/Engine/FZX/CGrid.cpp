// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <algorithm>
#include <cmath>
#include <unordered_set>

#include <glm/common.hpp>

#include "Include/Engine/FZX/CGrid.h"
#include "Include/Platform/SIMD.h"
#include "Include/Engine/FZX/Body.h"

namespace Borek {
namespace FZX {

static inline int ceil_div(float value, float divisor)
{
        return std::ceil(value / divisor);
}

static const SimdVec4i zero_vec4i(0, 0, 0, 0);
static const SimdVec4f zero_vec4f(0, 0, 0, 0);

CGrid::CGridNode::CGridNode(uint32_t element_id, uint32_t next)
        : element_id(element_id), next(next)  {}

                
CGrid::CGridElement::CGridElement(const SimdVec4f& rect, uint32_t entity_id)
        : rect(rect), entity_id(entity_id) {}

CGrid::CGrid(float x1, float y1, float x2, float y2,
             uint32_t cell_size_x, uint32_t cell_size_y)
{
        const int width = x2 - x1;
        const int height = y2 - y1;

        this->x = x1;
        this->y = y1;

        this->xy_vec = SimdVec4f(x1, y1, x1, y1);

        this->cols_count = ceil_div(width, cell_size_x);
        this->rows_count = ceil_div(height, cell_size_y);
        this->rc_count_vec = SimdVec4i(cols_count, rows_count, cols_count, rows_count);


        const float inv_cell_size_x = 1.0f / cell_size_x;
        const float inv_cell_size_y = 1.0f / cell_size_y;

        this->inv_cell_size_x = inv_cell_size_x;
        this->inv_cell_size_y = inv_cell_size_y;

        this->inv_cell_size_vec = SimdVec4f(inv_cell_size_x, inv_cell_size_y,
                                            inv_cell_size_x, inv_cell_size_y);

        this->cells.resize(cols_count * rows_count, UINT32_MAX);
}

void
CGrid::Insert(uint32_t entity_id, const SimdVec4f& rect)
{
        const SimdVec4i gpc = GetPositionInGrid(rect);
        uint32_t element_id = elements.insert(CGridElement(rect, entity_id));
        entity_element_map[entity_id] = element_id;

        for (int grid_y = gpc.y1; grid_y <= gpc.y2; grid_y++) {
                for (int grid_x = gpc.x1; grid_x <= gpc.x2; grid_x++) {
                        AddCellNode(GetCellIndex(grid_x, grid_y), element_id);
                }
        }
}

void
CGrid::Update(uint32_t entity_id, const SimdVec4f& new_rect)
{
        uint32_t element_id = entity_element_map[entity_id];
        const SimdVec4f& old_rect = elements[element_id].rect;
        const SimdVec4i gpc = GetPositionInGrid(old_rect);
        const SimdVec4i new_gpc = GetPositionInGrid(new_rect);
        std::unordered_set<uint32_t> old_cells;

        elements[element_id].rect = new_rect;

        for (int grid_y = gpc.y1; grid_y <= gpc.y2; grid_y++) {
                for (int grid_x = gpc.x1; grid_x <= gpc.x2; grid_x++) {
                        old_cells.insert(GetCellIndex(grid_x, grid_y));
                }
        }

        for (int grid_y = new_gpc.y1; grid_y <= new_gpc.y2; grid_y++) {
                for (int grid_x = new_gpc.x1; grid_x <= new_gpc.x2; grid_x++) {
                        uint32_t cell_id = GetCellIndex(grid_x, grid_y);
                        if (old_cells.contains(cell_id)) {
                                old_cells.erase(cell_id);
                        } else {
                                AddCellNode(cell_id, element_id);
                        }
                }
        }

        for (uint32_t cell_id : old_cells) {
                DeleteCellNode(cell_id, element_id);
        }
}

void
CGrid::Remove(uint32_t entity_id)
{
        uint32_t element_id = entity_element_map[entity_id];
        const SimdVec4i gpc = GetPositionInGrid(elements[element_id].rect);
        entity_element_map.erase(entity_id);

        for (int grid_y = gpc.y1; grid_y <= gpc.y2; grid_y++) {
                for (int grid_x = gpc.x1; grid_x <= gpc.x2; grid_x++) {
                        DeleteCellNode(GetCellIndex(grid_x, grid_y), entity_id);
                }
        }
}

// Get collisions between a rectangle and other colliders
void
CGrid::GetCollisions(const SimdVec4f& rect, uint32_t ignore_id,
                     SmallList<uint32_t>* res)
{
        const SimdVec4i gpc = GetPositionInGrid(rect);

        std::unordered_set<uint32_t> ignored_ids;

        for (int grid_y = gpc.y1; grid_y <= gpc.y2; grid_y++) {
                for (int grid_x = gpc.x1; grid_x <= gpc.x2; grid_x++) {
                        uint32_t node_id = cells[GetCellIndex(grid_x, grid_y)];

                        while (node_id != UINT32_MAX) {
                                const CGridNode& node = nodes[node_id];
                                if (!ignored_ids.contains(node.element_id)) {
                                        const CGridElement& element = elements[node.element_id];

                                        if (element.entity_id != ignore_id &&
                                            simd_rect_intersect4f(rect, element.rect)) {
                                                        res->push_back(element.entity_id);
                                        }

                                        ignored_ids.insert(node.element_id);
                                }

                                node_id = node.next;
                        }
                }
        }
}

void
CGrid::GetCollisions(glm::vec2 pos, uint32_t ignore_id, 
              SmallList<uint32_t>* res)
{
        const uint32_t grid_x = std::clamp((uint32_t)((pos.x - x) * inv_cell_size_x), 0u, cols_count - 1);
        const uint32_t grid_y = std::clamp((uint32_t)((pos.y - y) * inv_cell_size_y), 0u, rows_count - 1);
        const uint32_t index = GetCellIndex(grid_x, grid_y);
        const SimdVec4f rect(pos.x, pos.y, pos.x, pos.y);

        std::unordered_set<uint32_t> ignored_ids;

        uint32_t node_id = cells[index];
        while (node_id != UINT32_MAX) {
                const CGridNode& node = nodes[node_id];
                if (!ignored_ids.contains(node.element_id)) {
                        const CGridElement& element = elements[node.element_id];

                        if (element.entity_id != ignore_id &&
                            simd_rect_intersect4f(rect, element.rect)) {
                                        res->push_back(element.entity_id);
                        }

                        ignored_ids.insert(node.element_id);
                }

                node_id = node.next;
        }
}

bool
CGrid::RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir,
                 const SimdVec4f& target, RayCollisionData& out)
{
        const SimdVec4f svec = simd_sub4f(target, SimdVec4f(ray_origin.x, ray_origin.y,
                                                            ray_origin.x, ray_origin.y));
        glm::vec2 t_near = glm::vec2(svec.x1, svec.y1) / ray_dir;
        glm::vec2 t_far = glm::vec2(svec.x2, svec.y2) / ray_dir;

        if (std::isnan(t_far.x) || std::isnan(t_far.y) ||
                std::isnan(t_near.x) || std::isnan(t_near.y)) return false;

        if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
        if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

        if (t_near.x > t_far.y || t_near.y > t_far.x)
                return false;

        out.t_hit_near = std::max(t_near.x, t_near.y);
        float t_hit_far = std::min(t_far.x, t_far.y);

        if (t_hit_far < 0)
                return false;

        out.contact_point = ray_origin + out.t_hit_near * ray_dir;

        if (t_near.x > t_near.y) {
                if (ray_dir.x < 0) {
                        out.contact_normal = glm::vec2{ 1, 0 };
                } else {
                        out.contact_normal = glm::vec2{ -1, 0 };
                }
        } else {
                if (ray_dir.y < 0) {
                        out.contact_normal = glm::vec2{ 0, 1 };
                } else {
                        out.contact_normal = glm::vec2{ 0, -1 };
                }
        }

        return true;
}


glm::vec2
CGrid::MoveAndCollide(uint32_t entity_id, float dx, float dy, uint32_t& flags)
{
        if (dx == 0.0f && dy == 0.0f)
                return glm::vec2();

        flags &= ~(PhysicsFlags_CollidedBottom | PhysicsFlags_CollidedTop |
                PhysicsFlags_CollidedRight | PhysicsFlags_CollidedLeft);

        uint32_t element_id = entity_element_map[entity_id];
        const SimdVec4f& rect = elements[element_id].rect;

        const float h_width = (rect.x2 - rect.x1) / 2;
        const float h_height = (rect.y2 - rect.y1) / 2;
        const glm::vec2 center(rect.x1 + h_width, rect.y1 + h_height);
        const SimdVec4f half_size_vec(-h_width, -h_height, h_width, h_height);
        glm::vec2 ray_velocity(dx, dy);

        std::unordered_set<uint32_t> ignored_ids;
        const SimdVec4f moved_rect = simd_add4f(rect, simd_create4f(dx, dy, dx, dy));
        const SimdVec4i gpc = GetPositionInGrid(moved_rect);

        std::vector<std::pair<uint32_t, float>> elements_for_check;

        for (int grid_y = gpc.y1; grid_y <= gpc.y2; grid_y++) {
                for (int grid_x = gpc.x1; grid_x <= gpc.x2; grid_x++) {
                        uint32_t node_id = cells[GetCellIndex(grid_x, grid_y)];

                        while (node_id != UINT32_MAX) {
                                const CGridNode& node = nodes[node_id];
                                node_id = node.next;
                                if (ignored_ids.contains(node.element_id))
                                        continue;

                                const CGridElement& element = elements[node.element_id];
                                if (element.entity_id == entity_id)
                                        continue;

                                if (!simd_rect_intersect4f(moved_rect, element.rect))
                                        continue;

                                const float dx = std::max(element.rect.x1 - center.x, std::max(0.0f, center.x - element.rect.x2));
                                const float dy = std::max(element.rect.y1 - center.y, std::max(0.0f, center.y - element.rect.y2));
                                const float distance = std::sqrt(dx * dx + dy * dy);

                                elements_for_check.push_back({node.element_id, distance});

                                ignored_ids.insert(node.element_id);
                        }
                }
        }

        std::sort(elements_for_check.begin(), elements_for_check.end(), [](auto &left, auto &right) {
            return left.second < right.second;
        });

        for (auto& el : elements_for_check) {
                const CGridElement& element = elements[el.first];
                const SimdVec4f expanded_rect = simd_add4f(element.rect, half_size_vec);
                RayCollisionData data;

                if (RayVsRect(center, ray_velocity, expanded_rect, data)) {
                        if (data.t_hit_near > 1.0f || data.t_hit_near < -1.0f)
                                continue;

                        if (data.contact_normal.x != 0) {
                                if (data.contact_normal.x > 0) {
                                        flags |= PhysicsFlags_CollidedLeft;
                                } else {
                                        flags |= PhysicsFlags_CollidedRight;
                                }
                        } else {
                                if (data.contact_normal.y > 0) {
                                        flags |= PhysicsFlags_CollidedBottom;
                                } else {
                                        flags |= PhysicsFlags_CollidedTop;
                                }
                        }

                        ray_velocity += data.contact_normal *
                                glm::abs(ray_velocity) *
                                (1 - data.t_hit_near);
                }

        }


        SimdVec4f difference_vec(ray_velocity.x, ray_velocity.y,
                                 ray_velocity.x, ray_velocity.y);

        Update(entity_id, simd_add4f(rect, difference_vec));

        return ray_velocity;
}

bool
CGrid::Contains(uint32_t entity_id)
{
        return entity_element_map.contains(entity_id);
}

// Get Collisions between a point and other colliders
void
GetCollisions(float x, float y, SmallList<uint32_t>* res);

uint32_t
CGrid::GetCellIndex(uint32_t col, uint32_t row)
{
        return col + row * cols_count;
}

SimdVec4i
CGrid::GetPositionInGrid(const SimdVec4f& rect)
{
        const SimdVec4f moved_rect = simd_sub4f(rect, xy_vec);
        const SimdVec4f grid_pos_vec = simd_mul4f(moved_rect, inv_cell_size_vec);
        const SimdVec4i grid_pos_veci = simd_ftoi4f(grid_pos_vec);
        return simd_clamp4i(grid_pos_veci, zero_vec4i, rc_count_vec);
}

inline void
CGrid::AddCellNode(uint32_t cell_id, uint32_t element_id)
{
        uint32_t el_id = nodes.insert(CGridNode(element_id, cells[cell_id]));
        cells[cell_id] = el_id;
}

inline void
CGrid::DeleteCellNode(uint32_t cell_id, uint32_t element_id)
{
        uint32_t prev_node_id = UINT32_MAX;
        uint32_t node_id = cells[cell_id];

        while (nodes[node_id].element_id != element_id) {
                prev_node_id = node_id;
                node_id = nodes[node_id].next;
        }

        if (prev_node_id == UINT32_MAX) {
                cells[cell_id] = nodes[node_id].next;
        } else {
                nodes[prev_node_id].next = nodes[node_id].next;
        }

        nodes.erase(node_id);
}


} // namespace FZX
}  // namespace Borek
