// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <ostream>
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 400

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include "mruby/boxing_word.h"

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_utility.hpp>

#include <mrbcpp.h>

#include "Include/Base/Input.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBInput.h"
#include "Impl/Engine/Utils/StringHelpers.cpp"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static mrb_value keycodes;
static mrb_value mouse_buttons;


static Class vec2;

MRB_FUNC(InputGetAxis) {
        glm::vec2 axis = Borek::Input::GetAxis();
        mrb_value vec_coords[2];

        vec_coords[0] = MRB_FLOAT(axis.x);
        vec_coords[1] = MRB_FLOAT(axis.y);

        return mrb_class_new_instance(mrb, 2, vec_coords, vec2);
}

MRB_FUNC(InputGetMousePos) {
        glm::vec2 axis = Borek::Input::GetMousePos();
        mrb_value vec_coords[2];

        vec_coords[0] = MRB_FLOAT(axis.x);
        vec_coords[1] = MRB_FLOAT(axis.y);

        return mrb_class_new_instance(mrb, 2, vec_coords, vec2);
}

MRB_FUNC(InputGetMouseX) {
        return MRB_FLOAT(Borek::Input::GetMousePos().x);
}

MRB_FUNC(InputGetMouseY) {
        return MRB_FLOAT(Borek::Input::GetMousePos().x);
}

MRB_FUNC(InputIsKeyPressed) {
        int val = mrb_integer(mrb_hash_get(mrb, keycodes, MRB_ARG1));
        bool res = Borek::Input::IsKeyPressed(SCAST<KeyCode>(val));
        return MRB_BOOL(res);
}

MRB_FUNC(InputIsMouseButtonPressed) {
        int val = mrb_integer(mrb_hash_get(mrb, mouse_buttons, MRB_ARG1));
        bool res = Borek::Input::IsMouseButtonPressed(magic_enum::enum_value<MouseButton>(val));
        return MRB_BOOL(res);
}

void Input::Init(RubyEngine& engine)
{
        VM& mrb = engine.GetRubyVM();
        Module& borek = engine.GetBorekModule();

        vec2 = borek.get_class("Vec2");

        keycodes = mrb_hash_new(mrb);
        magic_enum::enum_for_each<KeyCode>([&mrb] (auto val) {
                constexpr KeyCode keycode = val;

                std::string key_code = to_snake_case(magic_enum::enum_name(keycode));
                mrb_value num = MRB_NUM(magic_enum::enum_integer<KeyCode>(val));

                mrb_hash_set(mrb, keycodes,
                             mrb_symbol_value(MRB_SYM(key_code.c_str())), num);
        });
        mrb_obj_freeze(mrb, keycodes);
        mouse_buttons = mrb_hash_new(mrb);
        magic_enum::enum_for_each<MouseButton>([&mrb] (auto val) {
                constexpr MouseButton mousebutton = val;

                std::string mouse_button = to_snake_case(magic_enum::enum_name(mousebutton));
                mrb_value num = MRB_NUM(magic_enum::enum_integer<MouseButton>(val));

                mrb_hash_set(mrb, mouse_buttons,
                             mrb_symbol_value(MRB_SYM(mouse_button.c_str())), num);
        });
        mrb_obj_freeze(mrb, mouse_buttons);

        borek.define_const("KEY_CODE", keycodes);
        borek.define_const("MOUSE_BUTTON", mouse_buttons);

        borek.define_class("Input")
                .define_class_method("axis", InputGetAxis)
                .define_class_method("mouse_pos", InputGetMousePos)
                .define_class_method("mouse_x", InputGetMouseX)
                .define_class_method("mouse_y", InputGetMouseY)
                .define_class_method("key_pressed?", InputIsKeyPressed,
                                     FuncArgs().Optional(1))
                .define_class_method("mouse_button_pressed?",
                                     InputIsMouseButtonPressed,
                                     FuncArgs().Optional(1));
}

}  // namespace RBModules
}  // namespace Borek
