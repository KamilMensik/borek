// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "mrbcpp.h"
#include <mruby.h>
#include <mruby/string.h>
#include "mruby/compile.h"

#include <Borek/Include/Base/Application.h>

#include "./ReplPanel.h"

namespace Borek {
namespace Panels {

Repl::Repl()
{
}

void Repl::OnImguiRender()
{
        static std::string res;
        static std::string input;

        if (ImGui::Begin("Repl##Repl", nullptr)) {
                ImGui::Text("%s", res.c_str());
                ImGui::InputTextMultiline("", &input);
                ImGui::SameLine();

                if (ImGui::Button("Send"))
                {
                        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
                        mrb_value val = mrb_load_string(mrb, input.c_str());
                        if (mrb->exc) {
                                BOREK_ENGINE_ERROR("Error while running MRB code");
                                mrb_print_error(mrb);
                        }
                        res = mrb_string_cstr(mrb, mrb_obj_as_string(mrb, val));
                        //input = "";
                }
        }

        ImGui::End();
}

}  // namespace Panels
}  // namespace Borek
