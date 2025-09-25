// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include <string>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

#include <ECS/World.h>

#include <Include/Base/Components.h>
#include <Include/Base/Renderer2D.h>
#include <Include/Base/Application.h>

#include "./Properties.h"

#define COMPONENT_ADD(_component) Application::SendEvent(new AddComponentEvent(ECS::GetId<_component>(), m_Entity));

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
        //auto normal_font = io.Fonts->Fonts[1];

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

static void Control(const char* label, glm::vec2& values,
                    float reset_value = 0.0f, float column_width = 100.0f)
{
        ImGuiIO io = ImGui::GetIO();
        auto bold_font = io.Fonts->Fonts[0];
        //auto normal_font = io.Fonts->Fonts[1];

        ImGui::PushID(label);
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, column_width);
        ImGui::Text("%s", label);
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
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
        ImGui::DragFloat("##X", &values.x, 1.0f, 0.F, 0.F, "%.1f");
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
        ImGui::DragFloat("##Y", &values.y, 1.0f, 0.F, 0.F, "%.1f");
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

template<class T>
static void DrawComponent(const char* label, Entity e, void(*fn)(Entity))
{
        if (e.HasComponent<T>()) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
                //float line_height = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
                //ImVec2 content_region_avail = ImGui::GetContentRegionAvail();
                ImGui::Separator();
                bool is_open = (ImGui::TreeNodeEx(VPCAST(ECS::GetId<T>()), tflags, "%s", label));
                ImGui::PopStyleVar();
                //ImGui::SameLine(content_region_avail.x - line_height * 0.5f);
                //if (ImGui::Button("«", ImVec2{line_height, line_height})) {
                //        ImGui::OpenPopup("Component Settings");
                //}

                if (is_open) {

                        //if (ImGui::BeginPopup("Component Settings")) {
                        //        ImGui::EndPopup();
                        //}

                        fn(e);

                        ImGui::TreePop();
                }
        }
}

void Properties::OnImguiRender()
{
        if (ImGui::Begin("Properties")) {
                if (m_Entity.IsNil()) {
                        ImGui::End();
                        return;
                }
                    
                //if (m_Entity.HasComponent<TagComponent>()) {
                //        if (ImGui::TreeNodeEx(VPCAST(ECS::GetId<TagComponent>()), tflags, "Tag")) {
                //                ImGui::TreePop();
                //        }
                //}
                auto& tc = m_Entity.GetComponent<TagComponent>();
                Control("Name", tc.value);
                ImGui::Separator();
                if (ImGui::TreeNodeEx(VPCAST(ECS::GetId<TransformComponent>()), tflags, "Transform")) {
                        auto& transform = m_Entity.GetComponent<TransformComponent>();
                        Control("Position", transform.position);
                        Control("Scale", transform.scale, 64.0f);
                        ImGui::InputFloat("Rotation", &transform.rotation);
                        ImGui::TreePop();
                }

                DrawComponent<ScriptComponent>("Script", m_Entity, [](Entity){
                        ImGui::Text("%s", "Has script yay");
                });

                DrawComponent<CameraComponent>("Camera", m_Entity, [](Entity e){
                        ImGui::Checkbox("IsActive", &e.GetComponent<CameraComponent>().is_active);
                });

                DrawComponent<SpriteComponent>("Sprite", m_Entity, [](Entity e){
                        auto& sprite = e.GetComponent<SpriteComponent>();
                        ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
                        auto sprite_texture = sprite.sprite ? sprite.sprite->GetTexture() : Renderer2D::WhiteTexture();
                        ImGui::ImageButton("SpriteImage", sprite_texture->GetId(), ImVec2{120, 120}, ImVec2(0, 1), ImVec2(1, 0));
                        if (ImGui::BeginDragDropTarget()) {
                                if (auto payload = ImGui::AcceptDragDropPayload("ASSET_ITEM")) {
                                        sprite.sprite = Sprite::Create(SCAST<char*>(payload->Data));
                                }
                                ImGui::EndDragDropTarget();
                        }
                        ImGui::TextWrapped("Sprite Texture");
                });

                DrawComponent<FZX::BodyComponent>("RigidBody", m_Entity,
                                                    [](Entity e){
                        auto& rb2d = e.GetComponent<FZX::BodyComponent>();
                        const char* types[] = { "Static", "Dynamic" };
                        const char* current_type = types[(int)rb2d.body_type];
                        if (ImGui::BeginCombo("Type", current_type)) {
                                for (int i = 0; i < 2; i++) {
                                        bool is_selected = current_type == types[i];
                                        if (ImGui::Selectable(types[i], is_selected)) {
                                                current_type = types[i];
                                                rb2d.body_type = (FZX::BodyType)i;
                                        }

                                        if (is_selected) {
                                                ImGui::SetItemDefaultFocus();
                                        }
                                }

                                ImGui::EndCombo();
                        }
                });

                DrawComponent<RubyScriptComponent>("RBScript", m_Entity, [](Entity e){
                        auto& rbscript = e.GetComponent<RubyScriptComponent>();  

                        if (ImGui::Button("Select Ruby Class")) {
                                ImGui::OpenPopup("Select Ruby Class");
                        }

                        if (ImGui::BeginPopup("Select Ruby Class")) {
                                for (auto cname : Application::GetRubyEngine().GetClasses()) {
                                        if (ImGui::MenuItem(cname.c_str())) {
                                                rbscript.ruby_class = cname;
                                        }
                                }
                                ImGui::EndPopup();
                        }
                });

                if (m_Entity.HasComponent<Text2DComponent>()) {
                        if (ImGui::TreeNodeEx(VPCAST(ECS::GetId<Text2DComponent>()), tflags, "Tag")) {
                                auto& tc = m_Entity.GetComponent<Text2DComponent>();
                                Control("Value", tc.value);
                                ImGui::InputFloat("Size", &tc.size);
                                ImGui::Text("Font ptr: %p", tc.font.get());
                                ImGui::TreePop();
                        }
                }
        };

        ImGui::End();
}

void Properties::ChangeEntity(Entity e)
{
        m_Entity = e;
}

}  // namespace Panels
}  // namespace Borek
