// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "glm/gtc/type_ptr.hpp"
#include <cstring>
#include <imgui.h>
#include <imgui_internal.h>
#include <Include/Base/Components.h>

#include "./Properties.h"

namespace Borek {
namespace Panels {

enum ComponentFlags {
        kDelete = 1,
};

Properties::Properties() : m_Entity() {}


static void Control(const char* label, glm::vec3& values,
                    float reset_value = 0.0f, float column_width = 100.0f)
{
        ImGuiIO io = ImGui::GetIO();
        auto bold_font = io.Fonts->Fonts[0];
        auto normal_font = io.Fonts->Fonts[1];

        ImGui::PushID(label);
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, column_width);
        ImGui::Text("%s", label);
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.0f, 0.0f});
        float line_height = GImGui->FontSize + GImGui->Style.FramePadding.y * 2;
        ImVec2 button_size = { line_height + 3.0f, line_height + 1.0f };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{198.0f / 256, 3.0f / 256, 0.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{220.0f / 256, 18.0f / 256, 15.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{255.0f / 256, 4.0f / 256, 0.0f / 256, 1.0f});
        ImGui::PushFont(bold_font);
        if (ImGui::Button("X", button_size))
                values.x = reset_value;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 1.0f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{94.0f / 256, 139.0f / 256, 38.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{119.0f / 256, 165.0f / 256, 40.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{164.0f / 256, 208.0f / 256, 105.0f / 256, 1.0f});
        ImGui::PushFont(bold_font);
        if (ImGui::Button("Y", button_size))
                values.y = reset_value;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 1.0f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{4.0f / 256, 51.0f / 256, 98.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{26.0f / 256, 91.0f / 256, 157.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{75.0f / 256, 125.0f / 256, 175.0f / 256, 1.0f});
        ImGui::PushFont(bold_font);
        if (ImGui::Button("Z", button_size))
                values.z = reset_value;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 1.0f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
}

static void Control(const char* label, String& value,
                    const char* reset_value = "", float column_width = 100.0f)
{
        ImGui::PushID(label);
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, column_width);
        ImGui::Text("%s", label);
        ImGui::NextColumn();

        char buf[256];
        std::strcpy(buf, value);
        ImGui::InputText("", buf, 256);
        value = buf;

        ImGui::Columns(1);

        ImGui::PopID();
}

static const ImGuiTreeNodeFlags tflags = ImGuiTreeNodeFlags_DefaultOpen
        | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_Framed
        | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

template<class T> requires std::derived_from<T, Component<T>>
static void DrawComponent(const char* label, Entity e, void(*fn)(Entity))
{
        if (e.HasComponent<T>()) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
                float line_height = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
                ImVec2 content_region_avail = ImGui::GetContentRegionAvail();
                bool delete_component = false;
                ImGui::Separator();
                bool is_open = (ImGui::TreeNodeEx(VPCAST(T::Id()), tflags, "%s", label));
                ImGui::PopStyleVar();
                ImGui::SameLine(content_region_avail.x - line_height * 0.5f);
                if (ImGui::Button("«", ImVec2{line_height, line_height})) {
                        ImGui::OpenPopup("Component Settings");
                }

                if (is_open) {

                        if (ImGui::BeginPopup("Component Settings")) {
                                if (ImGui::MenuItem("Remove component")) {
                                        delete_component = true;
                                        ImGui::CloseCurrentPopup();
                                }

                                ImGui::EndPopup();
                        }

                        fn(e);

                        ImGui::TreePop();
                }

                if (delete_component)
                        e.RemoveComponent<T>();
        }
}

void Properties::OnImguiRender()
{
        if (ImGui::Begin("Properties")) {
                if (m_Entity.IsNil()) {
                        ImGui::End();
                        return;
                }
                    
                if (m_Entity.HasComponent<TagComponent>()) {
                        if (ImGui::TreeNodeEx(VPCAST(TagComponent::Id()), tflags, "Tag")) {
                                auto& tc = m_Entity.GetComponent<TagComponent>();
                                Control("Value", tc.value);
                                ImGui::TreePop();
                        }
                }

                if (m_Entity.HasComponent<TransformComponent>()) {
                        ImGui::Separator();
                        if (ImGui::TreeNodeEx(VPCAST(TransformComponent::Id()), tflags, "Transform")) {
                                auto& transform = m_Entity.GetComponent<TransformComponent>();
                                transform.position *= 400.0f;
                                Control("Position", transform.position);
                                transform.position /= 400.0f;
                                transform.scale *= 400.0f;
                                Control("Scale", transform.scale, 64.0f);
                                transform.scale /= 400.0f;
                                Control("Rotation", transform.rotation);
                                ImGui::TreePop();
                        }
                }

                DrawComponent<ScriptComponent>("Script", m_Entity, [](Entity){
                        ImGui::Text("%s", "Has script yay");
                });

                DrawComponent<CameraComponent>("Camera", m_Entity, [](Entity){
                        ImGui::Text("%s", "Has camera yay");
                });

                DrawComponent<SpriteComponent>("Sprite", m_Entity, [](Entity e){
                        auto& sprite = e.GetComponent<SpriteComponent>();
                        ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
                });

                if (ImGui::Button("Add Component")) {
                        ImGui::OpenPopup("AddComponent");
                }

                if (ImGui::BeginPopup("AddComponent")) {
                        if (ImGui::MenuItem("Camera")) {
                                m_Entity.AddComponent<CameraComponent>();
                                ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::MenuItem("Sprite")) {
                                m_Entity.AddComponent<SpriteComponent>();
                                ImGui::CloseCurrentPopup();
                        }
                        if (ImGui::MenuItem("Script")) {
                                m_Entity.AddComponent<ScriptComponent>();
                                ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                }

                ImGui::End();
        };
}

void Properties::ChangeEntity(Entity e)
{
        m_Entity = e;
}

}  // namespace Panels
}  // namespace Borek
