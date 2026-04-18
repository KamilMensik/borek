// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Tools/MoveTool.h"
#include "Commands/EntityCommands.h"
#include "EditorSettings.h"
#include "Include/Base/Application.h"
#include "Include/Base/Colors.h"
#include "Include/Base/TransformCache.h"
#include "Include/Debug/Log.h"
#include "Include/Drawing/Quad.h"
#include "Include/Engine/Utils/GeometryUtils.h"
#include "Include/Engine/ZIndexAssigner.h"
#include "Include/Platform/SIMD.h"
#include "Misc/SceneTabBar.h"
#include "glm/common.hpp"
#include "magic_enum/magic_enum.hpp"

namespace Borek {

static inline bool
inside_xy_axis(const glm::vec2& epos, const glm::vec2& mpos)
{
        const float zoom = Application::GetCamera().first->zoom;
        const M128 mpos_r = simd_create4f(mpos.x, mpos.y, mpos.x, mpos.y);
        const M128 epos_r = simd_create4f(epos.x - 10 * zoom,
                                          epos.y - 10 * zoom,
                                          epos.x + 10 * zoom,
                                          epos.y + 10 * zoom);

        return simd_rect_intersect4f(mpos_r, epos_r);
}

static inline bool
inside_x_axis(const glm::vec2& epos, const glm::vec2& mpos)
{
        const float zoom = Application::GetCamera().first->zoom;
        const M128 mpos_r = simd_create4f(mpos.x, mpos.y, mpos.x, mpos.y);
        const M128 epos_r = simd_create4f(epos.x + zoom * 10,
                                          epos.y - zoom * 5,
                                          epos.x + zoom * 65,
                                          epos.y + zoom * 5);

        return simd_rect_intersect4f(mpos_r, epos_r);
}

static inline bool
inside_y_axis(const glm::vec2& epos, const glm::vec2& mpos)
{
        const float zoom = Application::GetCamera().first->zoom;
        const M128 mpos_r = simd_create4f(mpos.x, mpos.y, mpos.x, mpos.y);
        const M128 epos_r = simd_create4f(epos.x - zoom * 5,
                                          epos.y + zoom * 10,
                                          epos.x + zoom * 5,
                                          epos.y + zoom * 65);

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

bool
MoveTool::OnMousePressed(MouseButton button)
{
        if (m_State == State::kDisabled || m_CurrentEntity.IsNil() || !IsInsideViewport())
                return false;
        if (button != MouseButton::BUTTON_LEFT)
                return false;

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

        return true;
}

bool
MoveTool::OnMouseReleased(MouseButton button)
{
        if (m_State == State::kDisabled || m_CurrentEntity.IsNil())
                return false;

        if (m_State == State::kNothing)
                return false;

        if (button != MouseButton::BUTTON_LEFT)
                return false;

        m_State = State::kNothing;
        TransformComponent tc = m_CurrentEntity.Transform();
        m_CurrentEntity.Transform() = m_OldTransform;
        SceneTabBar::SendCommand<ModifyEntityTransformCommand>(m_CurrentEntity, tc);

        return true;
}

bool
MoveTool::Tick()
{
        if (m_State == State::kDisabled || m_CurrentEntity.IsNil())
                return false;

        const Entity parent = m_CurrentEntity.GetParent();
        const auto ptran =
                parent.IsNil() ? TransformComponent() : parent.GlobalTransform();
        auto& tran = m_CurrentEntity.Transform();
        const glm::vec2 gpos = Utils::Geometry::rotate_point(
                ptran.position, tran.position, ptran.rotation);
        const glm::vec2 mpos = Input::GetMouseWorldPos();

        const bool xyaxis = m_State == State::kXYAxis || inside_xy_axis(gpos, mpos);
        const bool xaxis = m_State == State::kXAxis || inside_x_axis(gpos, mpos);
        const bool yaxis = m_State == State::kYAxis || inside_y_axis(gpos, mpos);
        const float zoom = Application::GetCamera().first->zoom;

        Drawing::Quad::Draw(
                gpos - glm::vec2(10, 10) * zoom, glm::vec2(20, 20) * zoom,
                highlighted_color(Colors::WHITE, xyaxis),
                ZIndexAssigner::GetTop());

        Drawing::Quad::Draw(
                gpos + glm::vec2(10, -5) * zoom, glm::vec2(60, 10) * zoom,
                highlighted_color(Colors::RED, xaxis),
                ZIndexAssigner::GetTop());

        Drawing::Quad::Draw(
                gpos + glm::vec2(-5, 10) * zoom, glm::vec2(10, 60) * zoom,
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
                TransformCache::Invalidate(m_CurrentEntity);
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
