// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Tools/MoveTool.h"
#include "Commands/EntityCommands.h"
#include "EditorSettings.h"
#include "Include/Base/Colors.h"
#include "Include/Drawing/Quad.h"
#include "Include/Engine/SceneTree.h"
#include "Include/Engine/Utils/GeometryUtils.h"
#include "Include/Engine/ZIndexAssigner.h"
#include "Include/Platform/SIMD.h"
#include "Misc/SceneTabBar.h"
#include "glm/common.hpp"

namespace Borek {

static inline bool
inside_xy_axis(const glm::vec2& epos, const glm::vec2& mpos)
{
        const M128 mpos_r = simd_create4f(mpos.x, mpos.y, mpos.x, mpos.y);
        const M128 epos_r = simd_create4f(epos.x - 10, epos.y - 10, epos.x + 10,
                                          epos.y + 10);

        return simd_rect_intersect4f(mpos_r, epos_r);
}

static inline bool
inside_x_axis(const glm::vec2& epos, const glm::vec2& mpos)
{
        const M128 mpos_r = simd_create4f(mpos.x, mpos.y, mpos.x, mpos.y);
        const M128 epos_r = simd_create4f(epos.x + 10, epos.y - 5, epos.x + 65,
                                          epos.y + 5);

        return simd_rect_intersect4f(mpos_r, epos_r);
}

static inline bool
inside_y_axis(const glm::vec2& epos, const glm::vec2& mpos)
{
        const M128 mpos_r = simd_create4f(mpos.x, mpos.y, mpos.x, mpos.y);
        const M128 epos_r = simd_create4f(epos.x - 5, epos.y + 10, epos.x + 5,
                                          epos.y + 65);

        return simd_rect_intersect4f(mpos_r, epos_r);
}

static inline Color
highlighted_color(const Color& color, bool precondition)
{
        if (precondition)
                return color;

        Color res = color * 0.7;
        res.a = 255;
        return res;
}

void
MoveTool::OnMousePressed(MouseButton button)
{
        if (m_State == State::kDisabled || m_CurrentEntity.IsNil() || !IsInsideViewport())
                return;

        const glm::vec2 epos = m_CurrentEntity.GlobalTransform().position;
        const glm::vec2 mpos = Input::GetMouseWorldPos();
        m_Offset = epos - mpos;

        if (inside_xy_axis(epos, mpos)) {
                m_State = State::kXYAxis;
                m_OldTransform = m_CurrentEntity.Transform();
        } else if (inside_x_axis(epos, mpos)) {
                m_State = State::kXAxis;
                m_OldTransform = m_CurrentEntity.Transform();
        } else if (inside_y_axis(epos, mpos)) {
                m_State = State::kYAxis;
                m_OldTransform = m_CurrentEntity.Transform();
        } else {
                m_State = State::kNothing;
        }
}

void
MoveTool::OnMouseReleased(MouseButton button)
{
        if (m_State == State::kDisabled || m_CurrentEntity.IsNil())
                return;

        if (m_State == State::kNothing)
                return;

        m_State = State::kNothing;
        TransformComponent tc = m_CurrentEntity.Transform();
        m_CurrentEntity.Transform() = m_OldTransform;
        SceneTabBar::SendCommand<ModifyEntityComponentCommand<TransformComponent>>(m_CurrentEntity, tc);
}

bool
MoveTool::Tick()
{
        if (m_State == State::kDisabled || m_CurrentEntity.IsNil())
                return false;

        const auto ptran = m_CurrentEntity.GetParent().GlobalTransform();
        auto& tran = m_CurrentEntity.Transform();
        const glm::vec2 gpos = Utils::Geometry::rotate_point(
                ptran.position, tran.position, ptran.rotation);
        const glm::vec2 mpos = Input::GetMouseWorldPos();

        const bool xyaxis = m_State == State::kXYAxis || inside_xy_axis(gpos, mpos);
        const bool xaxis = m_State == State::kXAxis || inside_x_axis(gpos, mpos);
        const bool yaxis = m_State == State::kYAxis || inside_y_axis(gpos, mpos);

        Drawing::Quad::Draw(
                gpos - glm::vec2(10, 10), { 20, 20 },
                highlighted_color(Colors::WHITE, xyaxis),
                ZIndexAssigner::GetTop());

        Drawing::Quad::Draw(
                gpos + glm::vec2(10, -5), { 60, 10 },
                highlighted_color(Colors::RED, xaxis),
                ZIndexAssigner::GetTop());

        Drawing::Quad::Draw(
                gpos + glm::vec2(-5, 10), { 10, 60 },
                highlighted_color(Colors::GREEN, yaxis),
                ZIndexAssigner::GetTop());


        switch (m_State) {
        case State::kNothing:
        case State::kDisabled:
                break;
        case State::kXAxis:
                tran.position.x = m_Offset.x + mpos.x - ptran.position.x;
                break;
        case State::kYAxis:
                tran.position.y = m_Offset.y + mpos.y - ptran.position.y;
                break;
        case State::kXYAxis:
                tran.position = m_Offset + mpos - ptran.position;
                break;
        }

        if (m_State != State::kNothing) {
                glm::vec2 snap(EditorSettings::grid_snap.first,
                               EditorSettings::grid_snap.second);

                tran.position = glm::round(tran.position / snap) * snap;
        }

        return xyaxis | xaxis | yaxis;
}

void
MoveTool::Activate()
{
        if (m_State == State::kDisabled)
                m_State = State::kNothing;
}

void
MoveTool::Deactivate()
{
        m_State = State::kDisabled;
}

void
MoveTool::SetEntity(Entity e)
{
        m_CurrentEntity = e;
}

}  // namespace Borek
