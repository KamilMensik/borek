// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <algorithm>
#include <cmath>
#include <format>
#include <sstream>
#include <unordered_set>
#include <utility>

#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>

#include "Include/Engine/FZX/CGrid.h"
#include "Include/Platform/SIMD.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Engine/FZX/cute_c2.h"
#include "Include/Graphics/FrameBuffer.h"

#define TU(_thing) std::to_underlying(_thing)
#define CGRID_TYPE2(_type1, _type2) ((TU(_type1) << 4) + TU(_type2))

struct Collider {
        C2_TYPE type;
        union {
                c2Poly poly;
                c2Circle circle;
                c2AABB aabb;
        } val;

        operator std::string() const
        {
                switch (type) {
                case C2_TYPE_POLY: {
                        std::stringstream ss;
                        ss << "[POLY: [";
                        for (int i = 0; i < val.poly.count; i++) {
                                const auto& vert = val.poly.verts[i];
                                ss << "[" << vert.x << ", " << vert.y << "], ";
                        }
                        std::string res = ss.str();
                        res.pop_back();
                        res.pop_back();
                        res += ']';
                        return res;
                } case C2_TYPE_AABB: {
                        const c2AABB& aabb = val.aabb;
                        return std::format("[AABB: [[{}, {}], [{}, {}]]]",
                                           aabb.min.x, aabb.min.y, aabb.max.x, aabb.max.y);
                } default:
                        return "[UNKNOWN]";
                }
        }
};

namespace Borek {
namespace FZX {

static inline int ceil_div(float value, float divisor)
{
        return std::ceil(value / divisor);
}

static Collider
create_collider_poly(const CGrid::Element& el)
{
        Collider res;
        res.type = C2_TYPE_POLY;
        res.val.poly.count = 4;

        float4 points = simd_dump4f(el.aabb);

        if (el.rect_data.rotation == 0) {
                res.val.poly.verts[0] = { points.x, points.y };
                res.val.poly.verts[1] = { points.z, points.y };
                res.val.poly.verts[2] = { points.z, points.w };
                res.val.poly.verts[3] = { points.x, points.w };
                return res;
        }
        
        M128 s = simd_scalar4f(std::sin(glm::radians(el.rect_data.rotation)));
        M128 c = simd_scalar4f(std::cos(glm::radians(el.rect_data.rotation)));
        const float hw = el.rect_data.size_x / 2.0f;
        const float hh = el.rect_data.size_y / 2.0f;
        const float cx = (points.z + points.x) / 2.0f;
        const float cy = (points.w + points.y) / 2.0f;

        M128 xpoints = simd_create4f(hw, -hw, -hw, hw);
        M128 ypoints = simd_create4f(hh, hh, -hh, -hh);

        const float4 xres = simd_dump4f(simd_add4f(
                        simd_sub4f(simd_mul4f(xpoints, c),
                                   simd_mul4f(ypoints, s)),
                        simd_scalar4f(cx)));

        const float4 yres = simd_dump4f(simd_add4f(
                        simd_add4f(simd_mul4f(xpoints, s),
                                   simd_mul4f(ypoints, c)),
                        simd_scalar4f(cy)));

        res.val.poly.verts[0] = { xres.x, yres.x };
        res.val.poly.verts[1] = { xres.y, yres.y };
        res.val.poly.verts[2] = { xres.z, yres.z };
        res.val.poly.verts[3] = { xres.w, yres.w };

        c2MakePoly(&res.val.poly);
        return res;
}

static Collider
create_collider_aabb(const CGrid::Element& el)
{
        Collider res;
        res.type = C2_TYPE_AABB;
        float4 points = simd_dump4f(el.aabb);
        res.val.aabb = {
                .min = { points.x, points.y },
                .max = { points.z, points.w },
        };

        return res;
}

static Collider
create_collider(const CGrid::Element& el)
{
        switch (el.type) {
        case CGrid::Element::Type::Rect:
                return create_collider_poly(el);
        case CGrid::Element::Type::Tilemap:
                return create_collider_aabb(el);
                break;
        default:
                BOREK_ENGINE_ASSERT(false, "Circles not supported yet!");
                return create_collider_aabb(el);
        }
}

// Expects AABBs to already intersect
static bool
collides(const CGrid::Element& a, const CGrid::Element& b)
{
        using Type = CGrid::Element::Type;

        Collider ac;
        Collider bc;

        switch (CGRID_TYPE2(a.type, b.type)) {
        case CGRID_TYPE2(Type::Tilemap, Type::Tilemap):
                return true;
        default:
                break;
        }

        ac = create_collider(a);
        bc = create_collider(b);

        return c2Collided(&ac.val, nullptr, ac.type,
                          &bc.val, nullptr, bc.type);
}

static void
move(Collider& col, const glm::vec2& amount)
{
        //BOREK_ENGINE_WARN("Moving collider. by: [{}, {}], Before: {}",
        //                  amount.x, amount.y, std::string(col));
        switch (col.type) {
        case C2_TYPE_POLY:
                for (int i = 0; i < col.val.poly.count; i++) {
                        col.val.poly.verts[i].x += amount.x;               
                        col.val.poly.verts[i].y += amount.y;               
                }
                c2MakePoly(&col.val.poly);
                break;
        case C2_TYPE_AABB:
                col.val.aabb.min.x += amount.x;
                col.val.aabb.min.y += amount.y;
                col.val.aabb.max.x += amount.x;
                col.val.aabb.max.y += amount.y;
        default:
                return;
        }
        //BOREK_ENGINE_WARN("Moving collider. After: {}", std::string(col));
}

CGrid::CGridNode::CGridNode(uint32_t element_id, uint32_t next)
        : element_id(element_id), next(next) {}

CGrid::Element::Element(M128 aabb, uint32_t eid, ElementRectData&& rd)
        : aabb(aabb), entity_id(eid), type(Type::Rect), rect_data(rd) {}

CGrid::Element::Element(M128 aabb, uint32_t eid, ElementTilemapData&& td)
        : aabb(aabb), entity_id(eid), type(Type::Tilemap), tilemap_data(td) {}

CGrid::Element::Element(M128 aabb, uint32_t eid, ElementCircleData&& td)
        : aabb(aabb), entity_id(eid), type(Type::Circle), circle_data(td) {}

CGrid::CGrid(float x, float y, float z, float w,
             uint32_t cell_size_x, uint32_t cell_size_y)
{
        const int width = z - x;
        const int height = w - y;

        this->x = x;
        this->y = y;

        this->xy_vec = simd_create4f(x, y, x, y);

        this->cols_count = ceil_div(width, cell_size_x);
        this->rows_count = ceil_div(height, cell_size_y);
        this->rc_count_vec = simd_create4i(cols_count - 1, rows_count - 1, cols_count - 1, rows_count - 1);


        const float inv_cell_size_x = 1.0f / cell_size_x;
        const float inv_cell_size_y = 1.0f / cell_size_y;

        this->inv_cell_size_x = inv_cell_size_x;
        this->inv_cell_size_y = inv_cell_size_y;

        this->inv_cell_size_vec = simd_create4f(inv_cell_size_x, inv_cell_size_y,
                                                inv_cell_size_x, inv_cell_size_y);

        this->cells.resize(cols_count * rows_count, UINT32_MAX);
}

uint32_t
CGrid::Insert(uint32_t entity_id, Body& body, const glm::vec2& position,
              const glm::vec2& scale, float rotation)
{
        M128 aabb = body.Update(position, scale, rotation);
        const int4 gpc = GetPositionInGrid(aabb);

        uint32_t el_id;
        switch (body.collider_type) {
        case ColliderType::Rectangle: {
                ElementRectData data(body.rc.size_x * scale.x, body.rc.size_y * scale.y, 0);
                el_id = elements.insert(Element(aabb, entity_id, std::move(data)));
                break;
        }
        case ColliderType::Tilemap: {
                auto it = entity_element_map.find(entity_id);
                uint32_t next = it == entity_element_map.end() ? UINT32_MAX : it->second;
                ElementTilemapData data(body.tc.row, body.tc.col, next);
                el_id = elements.insert(Element(aabb, entity_id, std::move(data)));
                break;
        }
        case ColliderType::Circle: {
                ElementCircleData data(body.cc.radius * scale.x);
                el_id = elements.insert(Element(aabb, entity_id, std::move(data)));
                break;
        }
        default:
                el_id = 0;
                break;
        }

        entity_element_map[entity_id] = el_id;

        for (int grid_y = gpc.y; grid_y <= gpc.w; grid_y++) {
                for (int grid_x = gpc.x; grid_x <= gpc.z; grid_x++) {
                        AddCellNode(GetCellIndex(grid_x, grid_y), el_id);
                }
        }

        return el_id;
}

void
CGrid::Update(uint32_t entity_id, Body& body, const glm::vec2& position,
              const glm::vec2& scale, float rotation)
{
        M128 new_aabb = body.Update(position, scale, rotation);
        uint32_t element_id = entity_element_map[entity_id];
        M128 old_rect = elements[element_id].aabb;
        const int4 gpc = GetPositionInGrid(old_rect);
        const int4 new_gpc = GetPositionInGrid(new_aabb);
        std::unordered_set<uint32_t> old_cells;

        elements[element_id].aabb = new_aabb;

        for (int grid_y = gpc.y; grid_y <= gpc.w; grid_y++) {
                for (int grid_x = gpc.x; grid_x <= gpc.z; grid_x++) {
                        old_cells.insert(GetCellIndex(grid_x, grid_y));
                }
        }

        for (int grid_y = new_gpc.y; grid_y <= new_gpc.w; grid_y++) {
                for (int grid_x = new_gpc.x; grid_x <= new_gpc.z; grid_x++) {
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

        switch (body.collider_type) {
        case ColliderType::Rectangle: {
                ElementRectData& data = elements[element_id].rect_data;
                data.size_x = body.rc.size_x * scale.x;
                data.size_y = body.rc.size_y * scale.y;
                data.rotation = body.rc.rotation;
                break;
        }
        case ColliderType::Circle: {
                ElementCircleData& data = elements[element_id].circle_data;
                data.radius = body.cc.radius * scale.x;
                break;
        }
        default:
                break;
        }
}

void
CGrid::Update(uint32_t entity_id, M128 new_aabb)
{
        uint32_t element_id = entity_element_map[entity_id];
        M128 old_rect = elements[element_id].aabb;
        const int4 gpc = GetPositionInGrid(old_rect);
        const int4 new_gpc = GetPositionInGrid(new_aabb);
        std::unordered_set<uint32_t> old_cells;

        elements[element_id].aabb = new_aabb;

        for (int grid_y = gpc.y; grid_y <= gpc.w; grid_y++) {
                for (int grid_x = gpc.x; grid_x <= gpc.z; grid_x++) {
                        old_cells.insert(GetCellIndex(grid_x, grid_y));
                }
        }

        for (int grid_y = new_gpc.y; grid_y <= new_gpc.w; grid_y++) {
                for (int grid_x = new_gpc.x; grid_x <= new_gpc.z; grid_x++) {
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
        auto it = entity_element_map.find(entity_id);
        if (it == entity_element_map.end()) {
                return;
        }

        uint32_t element_id = it->second;
        Element& el = elements[element_id];
        const int4 gpc = GetPositionInGrid(el.aabb);
        entity_element_map.erase(entity_id);

        for (int grid_y = gpc.y; grid_y <= gpc.w; grid_y++) {
                for (int grid_x = gpc.x; grid_x <= gpc.z; grid_x++) {
                        DeleteCellNode(GetCellIndex(grid_x, grid_y), element_id);
                }
        }

        switch (el.type) {
        case Element::Type::Tilemap:
                while (element_id != UINT32_MAX) {
                        elements.erase(element_id);
                        element_id = elements[element_id].tilemap_data.next;
                }
                break;
        default:
                elements.erase(element_id);
                break;
        }
}

void
CGrid::RemoveTilemapNode(uint32_t entity_id, uint32_t element_id)
{
        Element& el = elements[element_id];
        const int4 gpc = GetPositionInGrid(el.aabb);
        entity_element_map.erase(entity_id);

        for (int grid_y = gpc.y; grid_y <= gpc.w; grid_y++) {
                for (int grid_x = gpc.x; grid_x <= gpc.z; grid_x++) {
                        DeleteCellNode(GetCellIndex(grid_x, grid_y), element_id);
                }
        }

        uint32_t next_id = el.tilemap_data.next;
        Element& next = elements[next_id];
        el.tilemap_data.row = next.tilemap_data.row;
        el.tilemap_data.col = next.tilemap_data.col;
        el.tilemap_data.next = next.tilemap_data.next;
        elements.erase(next_id);
}

// Get collisions between a rectangle and other colliders
void
CGrid::GetCollisions(const Element& rect, uint32_t ignore_id,
                     SmallList<uint32_t>* res)
{
        const int4 gpc = GetPositionInGrid(rect.aabb);

        std::unordered_set<uint32_t> ignored_ids = { ignore_id };

        for (int grid_y = gpc.y; grid_y <= gpc.w; grid_y++) {
                for (int grid_x = gpc.x; grid_x <= gpc.z; grid_x++) {
                        uint32_t node_id = cells[GetCellIndex(grid_x, grid_y)];

                        while (node_id != UINT32_MAX) {
                                const CGridNode& node = nodes[node_id];
                                const Element& element = elements[node.element_id];
                                if (ignored_ids.contains(element.entity_id)) {
                                        node_id = node.next;
                                        continue;
                                }

                                if (simd_rect_intersect4f(rect.aabb, element.aabb)) {
                                        if (collides(rect, element))
                                                res->push_back(element.entity_id);

                                        ignored_ids.insert(element.entity_id);
                                }

                                node_id = node.next;
                        }
                }
        }
}

void
CGrid::GetCollisions(uint32_t entity_id, SmallList<uint32_t>* res)
{
        Element& el = elements[entity_element_map[entity_id]];
        GetCollisions(el, entity_id, res);
}

void
CGrid::GetCollisions(glm::vec2 pos, uint32_t ignore_id, 
              SmallList<uint32_t>* res)
{
        const uint32_t grid_x = std::clamp((uint32_t)((pos.x - x) * inv_cell_size_x), 0u, cols_count - 1);
        const uint32_t grid_y = std::clamp((uint32_t)((pos.y - y) * inv_cell_size_y), 0u, rows_count - 1);
        const uint32_t index = GetCellIndex(grid_x, grid_y);
        const M128 rect = simd_create4f(pos.x, pos.y, pos.x, pos.y);

        std::unordered_set<uint32_t> ignored_ids = { ignore_id };

        uint32_t node_id = cells[index];
        while (node_id != UINT32_MAX) {
                const CGridNode& node = nodes[node_id];
                const Element& element = elements[node.element_id];
                if (ignored_ids.contains(element.entity_id)) {
                        node_id = node.next;
                        continue;
                }

                if (simd_rect_intersect4f(rect, element.aabb)) {
                        ignored_ids.insert(element.entity_id);
                        res->push_back(element.entity_id);
                }

                node_id = node.next;
        }
}

bool
CGrid::RayVsRect(const glm::vec2& ray_origin, const glm::vec2& ray_dir,
                 M128 target, RayCollisionData& out)
{
        const float4 svec = simd_dump4f(simd_sub4f(
                target, simd_create4f(ray_origin.x, ray_origin.y,
                                      ray_origin.x, ray_origin.y)));
        glm::vec2 t_near = glm::vec2(svec.x, svec.y) / ray_dir;
        glm::vec2 t_far = glm::vec2(svec.z, svec.w) / ray_dir;

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


std::pair<Collision, glm::vec2>
CGrid::MoveAndCollide(const Element& el, float dx, float dy, uint32_t& flags)
{
        flags &= ~(PhysicsFlags_CollidedBottom | PhysicsFlags_CollidedTop |
                PhysicsFlags_CollidedRight | PhysicsFlags_CollidedLeft);

        M128 aabb = simd_add4f(el.aabb, simd_create4f(dx, dy, dx, dy));
        const int4 gpc = GetPositionInGrid(aabb);

        auto it = entity_element_map.find(el.entity_id);
        std::unordered_set<uint32_t> ignored_ids;
        if (it != entity_element_map.end()) {
                ignored_ids.insert(it->second);
        }

        float4 el_rect = simd_dump4f(aabb);
        const glm::vec2 cA = glm::vec2(el_rect.x + el_rect.z,
                                       el_rect.y + el_rect.w) * 0.5f;
        std::vector<std::pair<const Element*, float>> found_collisions;
        for (int grid_y = gpc.y; grid_y <= gpc.w; grid_y++) {
                for (int grid_x = gpc.x; grid_x <= gpc.z; grid_x++) {
                        uint32_t node_id = cells[GetCellIndex(grid_x, grid_y)];

                        while (node_id != UINT32_MAX) {
                                const CGridNode& node = nodes[node_id];
                                const uint32_t element_id = node.element_id;
                                const Element& element = elements[element_id];
                                if (ignored_ids.contains(element_id)) {
                                        node_id = node.next;
                                        continue;
                                }

                                ignored_ids.insert(element_id);

                                if (simd_rect_intersect4f(aabb, element.aabb)) {
                                        const float4 rect = simd_dump4f(element.aabb);
                                        const glm::vec2 cB = glm::vec2(rect.x + rect.z,
                                                                       rect.y + rect.w) * 0.5f;
                                        const float dx = cA.x - cB.x;
                                        const float dy = cA.y - cB.y;
                                        const float distance = std::sqrt(dx * dx + dy * dy);
                                        found_collisions.emplace_back(&element, distance);
                                }

                                node_id = node.next;
                        }
                }
        }

        std::ranges::sort(found_collisions, [](auto& a, auto& b) {
                return a.second < b.second;
        });

        Collision res;

        Collider ac = create_collider(el);
        move(ac, {dx, dy});

        for (auto& [element, _distance] : found_collisions) {
                if (!simd_rect_intersect4f(aabb, element->aabb))
                        continue;

                Collider bc = create_collider(*element);

                c2Manifold col;
                c2Collide(&ac.val, nullptr, ac.type, &bc.val, nullptr, bc.type, &col);

                if (!col.count) continue;

                glm::vec2 pushback(col.n.x * (col.depths[0] + 0.001f),
                                   col.n.y * (col.depths[0] + 0.001f));

                dx -= pushback.x;
                dy -= pushback.y;


                simd_sub4f(aabb, simd_create4f(pushback.x, pushback.y, pushback.x, pushback.y));
                move(ac, -pushback);

                flags |= PhysicsFlags_CollidedBottom * (col.n.y < 0);
                flags |= PhysicsFlags_CollidedTop * (col.n.y > 0);
                flags |= PhysicsFlags_CollidedLeft * (col.n.x > 0);
                flags |= PhysicsFlags_CollidedRight * (col.n.x < 0);

                res.collider = element->entity_id;
                res.normal = glm::vec2(col.n.x, col.n.y);

                switch (element->type) {
                case CGrid::Element::Type::Tilemap:
                        res.tilemap_data.row = element->tilemap_data.row;
                        res.tilemap_data.col = element->tilemap_data.col;
                        break;
                default:
                        break;
                }
        }

        glm::vec2 res_vec(dx, dy);
        //BOREK_ENGINE_INFO("vec: [{}, {}], normal: [{}, {}], res_vec: [{}, {}]",
        //                  dx, dy, res.normal.x, res.normal.y, res_vec.x, res_vec.y);
        return { res, {dx, dy} };
}

bool
CGrid::Contains(uint32_t entity_id)
{
        return entity_element_map.contains(entity_id);
}

M128
CGrid::GetAABB(uint32_t entity_id)
{
        return elements[entity_element_map[entity_id]].aabb;
}

// Get Collisions between a point and other colliders
void
GetCollisions(float x, float y, SmallList<uint32_t>* res);

uint32_t
CGrid::GetCellIndex(uint32_t col, uint32_t row)
{
        return col + row * cols_count;
}

int4
CGrid::GetPositionInGrid(M128 rect)
{
        const M128 moved_rect = simd_sub4f(rect, xy_vec);
        const M128 grid_pos_vec = simd_mul4f(moved_rect, inv_cell_size_vec);
        const M128I grid_pos_veci = simd_ftoi4f(grid_pos_vec);
        return simd_dump4i(simd_clamp4i(grid_pos_veci, simd_zero4i(), rc_count_vec));
}

const CGrid::Element&
CGrid::GetElement(uint32_t entity_id)
{
        return elements[entity_element_map[entity_id]];
}

void
CGrid::Clear()
{
        cells.clear();
        cells.resize(cols_count * rows_count, UINT32_MAX);
        nodes.clear();
        elements.clear();
        entity_element_map.clear();
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
                if (node_id == UINT32_MAX) return;

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
