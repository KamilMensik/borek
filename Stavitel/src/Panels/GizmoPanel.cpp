// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include <imgui.h>
#include <imguismo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum/magic_enum.hpp>

#include <Borek/Include/Base/Components.h>
#include <Borek/Include/Base/Application.h>

#include "./GizmoPanel.h"

namespace Borek {
namespace Panels {

void GizmoPanel::DrawGizmo()
{
        if (!m_Entity.IsNil() && m_CurrentMode != -1) {
                ImGuizmo::SetOrthographic(true);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x,
                                  ImGui::GetWindowPos().y,
                                  ImGui::GetWindowWidth(),
                                  ImGui::GetWindowHeight());

                std::pair<CameraComponent*, TransformComponent*> cam_tran_data =
                        Application::Instance().GetCamera();

                glm::mat4 cam_projection =
                        cam_tran_data.first->GetProjectionMatrix();
                glm::mat4 cam_view =
                        glm::inverse(cam_tran_data.second->GetTransformMat());
                auto& trans = m_Entity.GetComponent<TransformComponent>();
                glm::mat4 trans_mat = trans.GetTransformMat();

                ImGuizmo::Manipulate(glm::value_ptr(cam_view),
                                     glm::value_ptr(cam_projection),
                                     SCAST<ImGuizmo::OPERATION>(m_CurrentMode),
                                     ImGuizmo::MODE::WORLD,
                                     glm::value_ptr(trans_mat));

                if (ImGuizmo::IsUsing()) {
                        ImGuizmo::DecomposeMatrixToComponents(
                                glm::value_ptr(trans_mat),
                                glm::value_ptr(trans.position),
                                glm::value_ptr(trans.rotation),
                                glm::value_ptr(trans.scale));
                }
        }
}

void GizmoPanel::ChangeEntity(Entity e)
{
        m_Entity = e;
}

void GizmoPanel::SetMode(Mode m)
{
        switch (m) {
        case Mode::kNothing:
                m_CurrentMode = -1;
                break;
        case Mode::kTranslate:
                m_CurrentMode = ImGuizmo::OPERATION::TRANSLATE;
                break;
        case Mode::kRotate:
                m_CurrentMode = ImGuizmo::OPERATION::ROTATE;
                break;
        case Mode::kScale:
                m_CurrentMode = ImGuizmo::OPERATION::SCALE;
                break;
        }
}

}  // namespace Panels
}  // namespace Borek
