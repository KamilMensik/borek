// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Input.h"
#include "Include/Base/TransformCache.h"
#include "Include/Components/ParticleComponent.h"
#include <cstdint>
#include <string>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/vector_float4.hpp>
#include <mrbcpp.h>
#include <ECS/World.h>

#include <Borek/Include/Base/Application.h>
#include <Borek/Include/Components/TransformComponent.h>
#include <Borek/Include/Components/TagComponent.h>
#include <Borek/Include/Components/IDComponent.h>
#include <Borek/Include/Components/Text2DComponent.h>
#include <Borek/Include/Components/SpriteComponent.h>
#include <Borek/Include/Components/RubyScriptComponent.h>
#include <Borek/Include/Components/ScriptComponent.h>
#include <Borek/Include/Base/Colors.h>
#include <Borek/Include/Base/MsgConnection.h>
#include <Borek/Include/Components/FZXComponents.h>
#include <Borek/Include/Components/SoundplayerComponent.h>
#include <Borek/Include/Components/TilemapComponent.h>
#include <Borek/Include/Components/AnimatedSpriteComponent.h>
#include <Borek/Include/Core.h>
#include <Borek/Include/Drawing/DrawingGlobals.h>
#include <Borek/Include/Engine/Assets/Asset.h>
#include <Borek/Include/Engine/Assets/TexAsset.h>
#include <Borek/Include/Engine/Assets/TilemapAsset.h>
#include <Borek/Include/Base/Symbol.h>
#include <Borek/Include/Components/ValueComponent.h>
#include <Borek/Include/Engine/Utils/StringHelpers.h>

#include "Misc/FontAwesome.h"
#include "Misc/SceneTabBar.h"
#include "Panels/PanelEvents.h"
#include "Popups/MsgConnectionPopup.h"
#include "Commands/EntityCommands.h"
#include "Panels/Properties.h"

namespace Borek {
namespace Panels {

struct EntityColor {
        Entity e;
        Color c;

        inline operator Color&()
        {
                return c;
        }

        void
        TryRefresh(Entity e, Color c)
        {
                if (!(Input::IsKeyPressed(KeyCode::LEFT_CONTROL) &&
                      (Input::IsKeyPressed(KeyCode::Y) ||
                       Input::IsKeyPressed(KeyCode::Z))) &&
                    this->e == e)
                        return;

                this->e = e;
                this->c = c;
        }
};

const float base_column_width = 100.0f;

Properties::Properties() : m_Entity()
{
        m_EventHandles[0] = ChangeEntityEvent::AddListener(EVENT_FN(OnChangeEntity));
}

Properties::~Properties()
{
        ChangeEntityEvent::RemoveListener(m_EventHandles[0]);
        s_TagInsert = Symbol();
}

static inline const Ref<Graphics::Texture2D>&
get_tex_safe(const SpriteComponent& sprite)
{
        if (sprite.texture.IsValid())
                return sprite.texture->texture;

        return Drawing::Globals::GetData().white_tex;
}

static inline const Ref<Graphics::Texture2D>&
get_tex_safe(const TilemapComponent& tmap)
{
        if (tmap.tilemap.IsValid() && tmap.tilemap->sprite_sheet.IsValid())
                return tmap.tilemap->sprite_sheet->texture;

        return Drawing::Globals::GetData().white_tex;
}

static bool control_modified = false;

static inline void
text(const char* text)
{
        ImGui::Text("%s", text);
}

static inline void
same_line()
{
        ImGui::SameLine();
}

static inline void
separator()
{
        ImGui::Separator();
}

static inline void
multi_items(int count)
{
        ImGui::PushMultiItemsWidths(count, ImGui::CalcItemWidth());
}

static inline bool
button(const char* label)
{
        ImGuiIO& io = ImGui::GetIO();
        auto bold_font = io.Fonts->Fonts[0];
        float line_height = GImGui->FontSize + GImGui->Style.FramePadding.y * 2;
        ImVec2 button_size = { line_height + 3.0f, line_height + 1.0f };

        ImGui::PushFont(bold_font);

        bool res = ImGui::Button(label, button_size);
        control_modified |= res;

        ImGui::PopFont();

        return res;
}

static bool
button_red(const char* label)
{

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{198.0f / 256, 3.0f / 256, 0.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{220.0f / 256, 18.0f / 256, 15.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{255.0f / 256, 4.0f / 256, 0.0f / 256, 1.0f});

        bool res = button(label);
        ImGui::PopStyleColor(3);

        return res;
}

static bool
button_green(const char* label)
{

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{94.0f / 256, 139.0f / 256, 38.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{119.0f / 256, 165.0f / 256, 40.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{164.0f / 256, 208.0f / 256, 105.0f / 256, 1.0f});

        bool res = button(label);
        ImGui::PopStyleColor(3);

        return res;
}

static bool
button_blue(const char* label)
{

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{4.0f / 256, 51.0f / 256, 98.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{26.0f / 256, 91.0f / 256, 157.0f / 256, 1.0f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{75.0f / 256, 125.0f / 256, 175.0f / 256, 1.0f});

        bool res = button(label);
        ImGui::PopStyleColor(3);

        return res;
}

static void
input(const char* label, float& val)
{
        ImGui::PushID(label);
        ImGui::InputFloat("", &val);
        control_modified |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::PopID();
}

static void
input(const char* label, double& val)
{
        ImGui::PushID(label);
        ImGui::InputDouble("", &val);
        control_modified |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::PopID();
}

static void
input(const char* label, int& val)
{
        ImGui::PushID(label);
        ImGui::InputInt("", &val);
        control_modified |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::PopID();
}

static void
input(const char* label, int64_t& val)
{
        ImGui::PushID(label);
        ImGui::InputScalar("", ImGuiDataType_S64, &val);
        control_modified |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::PopID();
}

static void
input(const char* label, uint32_t& val)
{
        ImGui::PushID(label);
        ImGui::InputScalar("", ImGuiDataType_U32, &val);
        control_modified |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::PopID();
}

static void
input(const char* label, uint16_t& val)
{
        ImGui::PushID(label);
        ImGui::InputScalar("", ImGuiDataType_U16, &val);
        control_modified |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::PopID();
}

static void
input(const char* label, Symbol& val)
{
        ImGui::PushID(label);
        std::string str = val;
        ImGui::InputText("", &str);
        if (ImGui::IsItemDeactivatedAfterEdit()) {
                if (str != val.Str()) {
                        val = str;
                        control_modified |= true;
                }
        }
        ImGui::PopID();
}

static void
input(const char* label, Symbol& val,
      const std::flat_map<Symbol, uint32_t>& values)
{
        ImGui::PushID(label);
        const char* current_type = val.Str().c_str();
        if (ImGui::BeginCombo("", current_type)) {
                for (auto it : values) {
                        bool is_selected = val == it.first;
                        if (ImGui::Selectable(it.first.Str().c_str(), is_selected)) {
                                val= it.first;
                        }

                        if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                        }
                }

                ImGui::EndCombo();
        }
        ImGui::PopID();
}

static void
input(const char* label, Color& val, Color& cache)
{
        ImGui::PushID(label);
        glm::vec4 col = cache;
        ImGui::ColorEdit4("", glm::value_ptr(col));
        cache = col;

        if (ImGui::IsItemDeactivatedAfterEdit() && val != cache) {
                control_modified = true;
                val = cache;
        }

        ImGui::PopID();
}

template <class T>
static void
input(const char* label, Asset<T>& val)
{
        ImGui::PushID(label);
        ImGui::Button(ICON_FA_FOLDER);
        if (ImGui::BeginDragDropTarget()) {
                if (auto payload = ImGui::AcceptDragDropPayload("FILE_EXPL_ITEM")) {
                        const char* pdata = SCAST<char*>(payload->Data);
                        std::string path = Utils::Path::ToRelative(pdata);
                        val = AssetManager::Get<T>(path);
                        control_modified = true;
                }
                ImGui::EndDragDropTarget();
        }
        ImGui::SameLine();
        ImGui::Text("%s", val.IsValid() ? val.GetPath().c_str() : "");
        ImGui::PopID();
}

static void
input(const char* label, MsgConnection& val, Entity from, uint32_t index)
{
        static std::pair<uint32_t, MsgConnection> locked_connection = { UINT32_MAX, {}};

        ImGui::PushID(label);
        Entity to(val.entity_id);
        if (to.IsNil())
                ImGui::Text("Nil");
        else
                ImGui::Text("%s[:%s]", from.PathTo(to).c_str(),
                            mrb_sym_name(Application::GetRubyEngine().GetRubyVM(), val.rbsym));


        ImGui::SameLine();
        if (val.IsNil()) {
                if (locked_connection.first == index && locked_connection.second != val) {
                        val = locked_connection.second;
                        control_modified = true;
                        locked_connection.first = UINT32_MAX;
                        ImGui::PopID();
                        return;
                }

                ImGui::PopID();
                if (!ImGui::Button(ICON_FA_LINK))
                        return;

                locked_connection = { index, val };
                Application::OpenPopup(new Popups::MsgConnectionPopup(from, &locked_connection.second));
                return;
        }

        ImGui::PopID();
        if (!ImGui::Button(ICON_FA_LINK_SLASH))
                return;

        val.rbsym = UINT32_MAX;
        val.entity_id = UINT32_MAX;
        control_modified = true;
}

static void
input(const char* label, bool& val)
{
        ImGui::PushID(label);
        ImGui::Checkbox("", &val);
        control_modified |= ImGui::IsItemDeactivatedAfterEdit();
        ImGui::PopID();
}

static void
control_begin(const char* label, float column_width)
{
        control_modified = false;

        ImGui::PushID(label);
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, column_width);
        ImGui::Text("%s", label);
        ImGui::NextColumn();
}

static bool
control_end()
{
        ImGui::Columns(1);
        ImGui::PopID();

        return control_modified;
}

static bool
control(const char* label, Symbol& value,
        std::string_view reset_value = "", float column_width = base_column_width)
{
        control_begin(label, column_width);

        input(label, value);

        return control_end();
}

static bool
control(const char* label, glm::vec2& value, float reset_value = 0.0f,
        float column_width = base_column_width)
{
        glm::vec2 old = value;

        control_begin(label, column_width);

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

        if (button_red("X"))
                value.x = reset_value;
        same_line();
        input("##X", value.x);

        same_line();
        if (button_green("Y"))
                value.y = reset_value;
        same_line();
        input("##Y", value.y);

        return control_end() && old != value;
}

template <size_t fixed_precision>
static bool
control(const char* label, glm::i16vec2& value, float reset_value = 0.0f,
        float column_width = base_column_width)
{
        constexpr float fp_mul = 1 << fixed_precision;
        constexpr float fp_mul_inv = 1.0f / fp_mul;

        glm::i16vec2 old = value;
        glm::vec2 res = glm::vec2(value) * fp_mul_inv;

        control_begin(label, column_width);

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

        if (button_red("X"))
                value.x = reset_value;
        same_line();
        input("##X", res.x);

        same_line();
        if (button_green("Y"))
                value.y = reset_value;
        same_line();
        input("##Y", res.y);

        value = glm::i16vec2(res * fp_mul);

        return control_end() && old != value;
}

template <size_t fixed_precision>
static bool
control(const char* label, glm::u16vec2& value, float reset_value = 0.0f,
        float column_width = base_column_width)
{
        constexpr float fp_mul = 1 << fixed_precision;
        constexpr float fp_mul_inv = 1.0f / fp_mul;

        glm::u16vec2 old = value;
        glm::vec2 res = glm::vec2(value) * fp_mul_inv;

        control_begin(label, column_width);

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

        if (button_red("X"))
                value.x = reset_value;
        same_line();
        input("##X", res.x);

        same_line();
        if (button_green("Y"))
                value.y = reset_value;
        same_line();
        input("##Y", res.y);

        value = glm::i16vec2(res * fp_mul);

        return control_end() && old != value;
}

static bool
control(const char* label, float& value, bool invert_result = false,
        float reset_value = 0.0f, float column_width = base_column_width)
{
        value = invert_result ? -value : value;
        float old = value;

        control_begin(label, column_width);

        input(label, value);
        same_line();
        if (button_red("X"))
                value = reset_value;

        bool modified = (old != value);
        if (invert_result)
                value *= -1;

        return control_end() && modified;
}

static bool
control(const char* label, int& value, float reset_value = 0.0f,
        float column_width = base_column_width)
{
        float old = value;

        control_begin(label, column_width);

        input(label, value);
        same_line();
        if (button_red("X"))
                value = reset_value;

        return control_end() && old != value;
}

static bool
control(const char* label, uint32_t& value, float reset_value = 0.0f,
        float column_width = base_column_width)
{
        float old = value;

        control_begin(label, column_width);

        input(label, value);
        same_line();
        if (button_red("X"))
                value = reset_value;

        return control_end() && old != value;
}

static bool
control(const char* label, uint16_t& value, float reset_value = 0.0f,
        float column_width = base_column_width)
{
        float old = value;

        control_begin(label, column_width);

        input(label, value);
        same_line();
        if (button_red("X"))
                value = reset_value;

        return control_end() && old != value;
}

static bool
control(const char* label, Color& value, Color& prev,
        const Color& reset_value = Colors::WHITE, float column_width = base_column_width)
{
        control_begin(label, column_width);

        input(label, value, prev);
        same_line();
        if (button_red("X"))
                value = reset_value;

        return control_end();
}

static bool
control(const char* label, bool& value, bool reset_value = false,
        float column_width = base_column_width)
{
        bool old = value;

        control_begin(label, column_width);

        input(label, value);

        return control_end() && old != value;
}

static bool
control(const char* label, BitFlags& flags, uint32_t flag,
        bool reset_value = false, float column_width = base_column_width)
{
        bool value = flags.HasFlags(flag);

        control_begin(label, column_width);

        input(label, value);

        flags.SetFlags(flag, value);

        return control_end();
}

static bool
control(const char* label, Symbol& value,
        const std::flat_map<Symbol, uint32_t>& values,
        bool reset_value = false, float column_width = base_column_width)
{
        Symbol old = value;

        control_begin(label, column_width);

        input(label, value, values);

        return control_end() && old != value;
}

template <class T>
static bool
control(const char* label, Asset<T>& value, Asset<T> reset_value = Asset<T>(),
        float column_width = base_column_width)
{
        Asset<T> old = value;

        control_begin(label, column_width);

        input(label, value);

        return control_end() && old != value;
}

static bool
control(const char* label, MsgConnection& value, Entity from, uint32_t index,
        float column_width = base_column_width)
{
        MsgConnection old = value;

        control_begin(label, column_width);

        input(label, value, from, index);

        return control_end() && old != value;
}

static bool
control(const char* label, TagComponent& tc, Symbol& write,
        float column_width = base_column_width)
{
        control_begin(label, column_width);

        input("Add Tag", write);
        same_line();
        if (button(ICON_FA_PLUS)) {
                tc.AddTag(write);
                write = "";
        }

        ImGui::Columns(1);
        separator();

        for (auto& sym : tc) {
                ImGui::PushID(sym.Value());
                input("", sym);
                if (sym == "")
                        tc.RemoveTag(sym);

                same_line();

                if (button(ICON_FA_X))
                        tc.RemoveTag(sym);
                ImGui::PopID();
        }

        return control_end();
}

static bool
control(const char* label, ValueComponent& vc, Symbol& val_name, Symbol& val_type,
        Entity e, float column_width = base_column_width)
{
        static const std::array<Symbol, 7> value_types = {
                "Number", "Int", "String", "Bool", "Color", "Node", "Asset",
        };

        control_begin(label, column_width);
        multi_items(2);

        input("Add", val_name);

        same_line();

        const char* current_type = val_type.Str().c_str();
        if (ImGui::BeginCombo("", current_type)) {
                for (auto it : value_types) {
                        bool is_selected = val_type == it;
                        if (ImGui::Selectable(it.Str().c_str(), is_selected)) {
                                val_type = it;
                        }

                        if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                        }
                }

                ImGui::EndCombo();
        }

        same_line();
        if (button(ICON_FA_PLUS)) {
                ValueType_ type;

                switch (Hash(val_type.Str())) {
                case Hash("Int"):
                        type = ValueType_Int;
                        break;
                case Hash("Number"):
                        type = ValueType_Number;
                        break;
                case Hash("String"):
                        type = ValueType_String;
                        break;
                case Hash("Bool"):
                        type = ValueType_Bool;
                        break;
                case Hash("Color"):
                        type = ValueType_Color;
                        break;
                case Hash("Node"):
                        type = ValueType_Node;
                        break;
                case Hash("Asset"):
                        type = ValueType_Asset;
                        break;
                default: {
                        type = ValueType_Int;
                        break;
                }
                }

                vc.Add(val_name, type);
                val_name = "";
        }

        ImGui::Columns(1);
        separator();

        for (auto& val : vc) {
                ImGui::PushID(val.name.Value());
                input("val_name", val.name);
                if (val.name == "")
                        vc.Remove(val.name);

                same_line();

                switch (val.type) {
                case ValueType_Number:
                        input("num", val.number);
                        break;
                case ValueType_Int:
                        input("int", val.integer);
                        break;
                case ValueType_String:
                        input("str", val.str);
                        break;
                case ValueType_Bool:
                        input("bool", val.boolean);
                        break;
                case ValueType_Color:
                        static Map<uint32_t, Color> cached_colors;
                        static Entity cached_color_entity;

                        if (cached_color_entity != e ||
                            (Input::IsKeyPressed(KeyCode::LEFT_CONTROL) &&
                             (Input::IsKeyPressed(KeyCode::Y) ||
                              Input::IsKeyPressed(KeyCode::Z)))) {
                                cached_color_entity = e;
                                cached_colors.clear();
                        }

                        cached_colors.try_emplace(val.name, val.color);
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(ICON_FA_X).x - ImGui::GetStyle().FramePadding.x * 4.f);
                        input("color", val.color, cached_colors[val.name]);
                        ImGui::PopItemWidth();
                        break;
                case ValueType_Node:
                        bool test;
                        input("bool", test);
                        break;
                case ValueType_Asset:
                        bool test2;
                        input("bool", test2);
                        break;
                }

                same_line();

                if (button(ICON_FA_X))
                        vc.Remove(val.name);
                ImGui::PopID();
        }

        return control_end();
}

void Properties::OnImguiRender()
{
        if (!ImGui::Begin("Properties") || m_Entity.IsNil()) {
                ImGui::End();
                return;
        }

        Symbol name = m_Entity.GetNameSym();
        if (control("Name", name)) {
                SceneTabBar::SendCommand<RenameEntityCommand>(m_Entity, name);
        }

        DrawComponent<TagComponent>("Tags", [](Entity e){
                using cmd = ModifyEntityComponentCommand<TagComponent>;

                auto tc = e.GetComponent<TagComponent>();

                if (control("Add", tc, s_TagInsert))
                        SceneTabBar::SendCommand<cmd>(e, tc);
        });

        DrawComponent<ValueComponent>("Values", [](Entity e) {
                using cmd = ModifyEntityComponentCommand<ValueComponent>;

                static Symbol val_name;
                static Symbol val_type;

                auto vc = e.GetComponent<ValueComponent>();

                if (control("Add", vc, val_name, val_type, e))
                        SceneTabBar::SendCommand<cmd>(e, vc);
        }, false);
        if (!(m_Entity.HasComponent<PrefabComponent>() ||
              m_Entity.HasComponent<ValueComponent>())) {
                if (button_green("Add Values"))
                        m_Entity.AddComponent<ValueComponent>();
        }

        DrawComponent<TransformComponent>("Transform", [](Entity e){
                using cmd = ModifyEntityTransformCommand;

                bool modified = false;
                auto transform = e.GetComponent<TransformComponent>();
                modified |= control("Position", transform.position);
                modified |= control<6>("Scale", transform.scale, 1.0f);
                modified |= control<6>("Pivot", transform.pivot, 1.0f);
                modified |= control("Rotation", transform.rotation, true);

                if (modified) {
                        SceneTabBar::SendCommand<cmd>(e, transform);
                        TransformCache::Invalidate(e);
                }
        }, false);

        DrawComponent<ScriptComponent>("Script", [](Entity){
                ImGui::Text("%s", "Has script yay");
        }, false);

        DrawComponent<CameraComponent>("Camera", [](Entity e){
                using cmd = ModifyEntityComponentCommand<CameraComponent>;

                auto cam = e.GetComponent<CameraComponent>();
                bool modified = false;

                modified |= control("Is Active", cam.is_active);
                modified |= control("Width", cam.width);
                modified |= control("Height", cam.height);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, cam);
        });

        DrawComponent<SpriteComponent>("Sprite", [](Entity e){
                using cmd = ModifyEntityComponentCommand<SpriteComponent>;

                auto sprite = e.GetComponent<SpriteComponent>();
                static EntityColor cached_color;
                bool modified = false;

                cached_color.TryRefresh(e, sprite.color);

                modified |= control("FlipX", sprite.flags, SpriteComponentFlags_FlipX);
                modified |= control("FlipY", sprite.flags, SpriteComponentFlags_FlipY);
                modified |= control("Color", sprite.color, cached_color);
                modified |= control("Size X", sprite.size_x);
                modified |= control("Size Y", sprite.size_y);
                modified |= control("Texture", sprite.texture);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, sprite);

                auto sprite_texture = get_tex_safe(sprite);
                ImGui::Image(sprite_texture->GetId(), ImVec2{120, 120},
                             ImVec2(0, 1), ImVec2(1, 0));
        });

        DrawComponent<BodyComponent>("RigidBody", [](Entity e){
                using cmd = ModifyEntityComponentCommand<BodyComponent>;

                auto rb2d = e.GetComponent<BodyComponent>();
                bool modified = false;

                modified |= control("Size X", rb2d.rc.size_x);
                modified |= control("Size Y", rb2d.rc.size_y);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, rb2d);
        });

        DrawComponent<AreaComponent>("Area", [](Entity e){
                using cmd = ModifyEntityComponentCommand<AreaComponent>;

                auto area = e.GetComponent<AreaComponent>();
                bool modified = false;

                modified |= control("Size X", area.rc.size_x);
                modified |= control("Size Y", area.rc.size_y);
                modified |= control("On Area Entered", area.callbacks->on_area_entered, e, 0, 175.0f);
                modified |= control("On Area Exited", area.callbacks->on_area_exited, e, 1, 175.0f);
                modified |= control("On Body Entered", area.callbacks->on_body_entered, e, 2, 175.0f);
                modified |= control("On Body Exited", area.callbacks->on_body_exited, e, 3, 175.0f);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, area);
        });

        DrawComponent<RubyScriptComponent>("RBScript", [](Entity e){
                using cmd = ModifyEntityComponentCommand<RubyScriptComponent>;

                auto rbscript = e.GetComponent<RubyScriptComponent>();  
                bool modified = false;

                modified |= control("Script", rbscript.script);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, rbscript);
        }, false);

        DrawComponent<Text2DComponent>("Font", [](Entity e){
                using cmd = ModifyEntityComponentCommand<Text2DComponent>;

                auto txt = e.GetComponent<Text2DComponent>();  

                static EntityColor cached_color; 
                cached_color.TryRefresh(e, txt.color);

                bool modified = false;

                modified |= control("Text", txt.value);
                modified |= control("Script", txt.font);
                modified |= control("Size", txt.size);
                modified |= control("Color", txt.color, cached_color);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, txt);
        }, false);

        DrawComponent<TilemapComponent>("Tilemap", [](Entity e){
                using cmd = ModifyEntityComponentCommand<TilemapComponent>;

                auto tc = e.GetComponent<TilemapComponent>();
                bool modified = false;

                modified |= control("Tilemap", tc.tilemap);
                modified |= control("Step X", tc.step_x);
                modified |= control("Step Y", tc.step_y);

                auto tilemap_texture = get_tex_safe(tc);
                ImGui::Image(tilemap_texture->GetId(), ImVec2{120, 120},
                             ImVec2(0, 1), ImVec2(1, 0));

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, tc);
        });

        DrawComponent<SoundPlayerComponent>("SoundPlayer", [](Entity e){
                using cmd = ModifyEntityComponentCommand<SoundPlayerComponent>;

                auto snd = e.GetComponent<SoundPlayerComponent>();  
                bool modified = false;

                modified |= control("Audio", snd.asset);
                modified |= control("Autoplay", snd.flags, SoundPlayerFlags_Autoplay);
                modified |= control("OnFinished", *snd.on_finished, e, 0);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, snd);
        });

        DrawComponent<AnimatedSpriteComponent>("AnimatedSprite", [](Entity e){
                using cmd = ModifyEntityComponentCommand<AnimatedSpriteComponent>;

                auto spr = e.GetComponent<AnimatedSpriteComponent>();  
                bool modified = false;

                modified |= control("Animation", spr.anim);
                modified |= control("FlipX", spr.flags, AnimatedSpriteComponentFlags_FlipX);
                modified |= control("FlipY", spr.flags, AnimatedSpriteComponentFlags_FlipY);
                modified |= control("Repeat", spr.flags, AnimatedSpriteComponentFlags_Repeat);
                modified |= control("Size X", spr.size_x);
                modified |= control("Size Y", spr.size_y);
                if (spr.anim.IsValid()) {
                        modified |= control("Selected Anim",
                                            spr.current_animation,
                                            spr.anim->animation_by_name);
                }

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, spr);
        });

        DrawComponent<ParticleComponent>("ParticleEmmiter", [](Entity e){
                using cmd = ModifyEntityComponentCommand<ParticleComponent>;

                auto particle = e.GetComponent<ParticleComponent>();  
                bool modified = false;

                static EntityColor cached_from_color; 
                static EntityColor cached_to_color; 
                cached_from_color.TryRefresh(e, particle.from_color);
                cached_to_color.TryRefresh(e, particle.to_color);

                modified |= control("Emmits Per Second", particle.emmits_per_second);
                modified |= control("Lifetime", particle.lifetime);
                modified |= control("From Size", particle.from_size);
                modified |= control("To Size", particle.to_size);
                modified |= control("Size Variation", particle.size_variation);
                modified |= control("From Color", particle.from_color, cached_from_color);
                modified |= control("To Color", particle.to_color, cached_to_color);
                modified |= control("Velocity", particle.velocity);
                modified |= control("Velocity Variation", particle.velocity_variation);
                modified |= control("Enabled", particle.enabled);

                if (modified)
                        SceneTabBar::SendCommand<cmd>(e, particle);
        });

        ImGui::End();
}

bool Properties::OnChangeEntity(ChangeEntityEvent& e)
{
        m_Entity = e.GetEntity();
        s_TagInsert = "";

        return true;
}

Symbol Properties::s_TagInsert;

}  // namespace Panels
}  // namespace Borek
