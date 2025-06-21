#include "Include/Base/Colors.h"
#include "Include/Debug/Log.h"
#include "Include/Events/Event.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Include/ImGui/ImGuiLayer.h"
#include "Include/ImGui/ImGuiImpl.h"
#include "Include/Base/Application.h"
#include "Include/Base/Input.h"
#include "imguismo/ImGuizmo.h"

#ifdef WIN32
#define TRY_ENABLE_VIEWPORTS //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#else
#include <stdlib.h>
#define TRY_ENABLE_VIEWPORTS                                               \
         if (strcmp(secure_getenv("XDG_SESSION_TYPE"), "x11") == 0) {      \
                      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  \
         }                                                             
#endif

namespace Borek {

ImGuiLayer::ImGuiLayer() : Layer("ImGui")
{
}
ImGuiLayer::~ImGuiLayer()
{
}

ImGuiKey KeyToImguiKey(KeyCode keycode)
{
    switch (keycode)
    {
        case KeyCode::TAB: return ImGuiKey_Tab;
        case KeyCode::LEFT: return ImGuiKey_LeftArrow;
        case KeyCode::RIGHT: return ImGuiKey_RightArrow;
        case KeyCode::UP: return ImGuiKey_UpArrow;
        case KeyCode::DOWN: return ImGuiKey_DownArrow;
        case KeyCode::PAGE_UP: return ImGuiKey_PageUp;
        case KeyCode::PAGE_DOWN: return ImGuiKey_PageDown;
        case KeyCode::HOME: return ImGuiKey_Home;
        case KeyCode::END: return ImGuiKey_End;
        case KeyCode::INSERT: return ImGuiKey_Insert;
        case KeyCode::DEL: return ImGuiKey_Delete;
        case KeyCode::BACKSPACE: return ImGuiKey_Backspace;
        case KeyCode::SPACE: return ImGuiKey_Space;
        case KeyCode::ENTER: return ImGuiKey_Enter;
        case KeyCode::ESCAPE: return ImGuiKey_Escape;
        case KeyCode::APOSTROPHE: return ImGuiKey_Apostrophe;
        case KeyCode::COMMA: return ImGuiKey_Comma;
        case KeyCode::MINUS: return ImGuiKey_Minus;
        case KeyCode::PERIOD: return ImGuiKey_Period;
        case KeyCode::SLASH: return ImGuiKey_Slash;
        case KeyCode::SEMICOLON: return ImGuiKey_Semicolon;
        case KeyCode::EQUAL: return ImGuiKey_Equal;
        case KeyCode::LEFT_BRACKET: return ImGuiKey_LeftBracket;
        case KeyCode::BACKSLASH: return ImGuiKey_Backslash;
        case KeyCode::WORLD_1: return ImGuiKey_Oem102;
        case KeyCode::WORLD_2: return ImGuiKey_Oem102;
        case KeyCode::RIGHT_BRACKET: return ImGuiKey_RightBracket;
        case KeyCode::GRAVE_ACCENT: return ImGuiKey_GraveAccent;
        case KeyCode::CAPS_LOCK: return ImGuiKey_CapsLock;
        case KeyCode::SCROLL_LOCK: return ImGuiKey_ScrollLock;
        case KeyCode::NUM_LOCK: return ImGuiKey_NumLock;
        case KeyCode::PRINT_SCREEN: return ImGuiKey_PrintScreen;
        case KeyCode::PAUSE: return ImGuiKey_Pause;
        case KeyCode::KP_0: return ImGuiKey_Keypad0;
        case KeyCode::KP_1: return ImGuiKey_Keypad1;
        case KeyCode::KP_2: return ImGuiKey_Keypad2;
        case KeyCode::KP_3: return ImGuiKey_Keypad3;
        case KeyCode::KP_4: return ImGuiKey_Keypad4;
        case KeyCode::KP_5: return ImGuiKey_Keypad5;
        case KeyCode::KP_6: return ImGuiKey_Keypad6;
        case KeyCode::KP_7: return ImGuiKey_Keypad7;
        case KeyCode::KP_8: return ImGuiKey_Keypad8;
        case KeyCode::KP_9: return ImGuiKey_Keypad9;
        case KeyCode::KP_DECIMAL: return ImGuiKey_KeypadDecimal;
        case KeyCode::KP_DIVIDE: return ImGuiKey_KeypadDivide;
        case KeyCode::KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case KeyCode::KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case KeyCode::KP_ADD: return ImGuiKey_KeypadAdd;
        case KeyCode::KP_ENTER: return ImGuiKey_KeypadEnter;
        case KeyCode::KP_EQUAL: return ImGuiKey_KeypadEqual;
        case KeyCode::LEFT_SHIFT: return ImGuiKey_LeftShift;
        case KeyCode::LEFT_CONTROL: return ImGuiKey_LeftCtrl;
        case KeyCode::LEFT_ALT: return ImGuiKey_LeftAlt;
        case KeyCode::LEFT_SUPER: return ImGuiKey_LeftSuper;
        case KeyCode::RIGHT_SHIFT: return ImGuiKey_RightShift;
        case KeyCode::RIGHT_CONTROL: return ImGuiKey_RightCtrl;
        case KeyCode::RIGHT_ALT: return ImGuiKey_RightAlt;
        case KeyCode::RIGHT_SUPER: return ImGuiKey_RightSuper;
        case KeyCode::MENU: return ImGuiKey_Menu;
        case KeyCode::NUM_0: return ImGuiKey_0;
        case KeyCode::NUM_1: return ImGuiKey_1;
        case KeyCode::NUM_2: return ImGuiKey_2;
        case KeyCode::NUM_3: return ImGuiKey_3;
        case KeyCode::NUM_4: return ImGuiKey_4;
        case KeyCode::NUM_5: return ImGuiKey_5;
        case KeyCode::NUM_6: return ImGuiKey_6;
        case KeyCode::NUM_7: return ImGuiKey_7;
        case KeyCode::NUM_8: return ImGuiKey_8;
        case KeyCode::NUM_9: return ImGuiKey_9;
        case KeyCode::A: return ImGuiKey_A;
        case KeyCode::B: return ImGuiKey_B;
        case KeyCode::C: return ImGuiKey_C;
        case KeyCode::D: return ImGuiKey_D;
        case KeyCode::E: return ImGuiKey_E;
        case KeyCode::F: return ImGuiKey_F;
        case KeyCode::G: return ImGuiKey_G;
        case KeyCode::H: return ImGuiKey_H;
        case KeyCode::I: return ImGuiKey_I;
        case KeyCode::J: return ImGuiKey_J;
        case KeyCode::K: return ImGuiKey_K;
        case KeyCode::L: return ImGuiKey_L;
        case KeyCode::M: return ImGuiKey_M;
        case KeyCode::N: return ImGuiKey_N;
        case KeyCode::O: return ImGuiKey_O;
        case KeyCode::P: return ImGuiKey_P;
        case KeyCode::Q: return ImGuiKey_Q;
        case KeyCode::R: return ImGuiKey_R;
        case KeyCode::S: return ImGuiKey_S;
        case KeyCode::T: return ImGuiKey_T;
        case KeyCode::U: return ImGuiKey_U;
        case KeyCode::V: return ImGuiKey_V;
        case KeyCode::W: return ImGuiKey_W;
        case KeyCode::X: return ImGuiKey_X;
        case KeyCode::Y: return ImGuiKey_Y;
        case KeyCode::Z: return ImGuiKey_Z;
        case KeyCode::F1: return ImGuiKey_F1;
        case KeyCode::F2: return ImGuiKey_F2;
        case KeyCode::F3: return ImGuiKey_F3;
        case KeyCode::F4: return ImGuiKey_F4;
        case KeyCode::F5: return ImGuiKey_F5;
        case KeyCode::F6: return ImGuiKey_F6;
        case KeyCode::F7: return ImGuiKey_F7;
        case KeyCode::F8: return ImGuiKey_F8;
        case KeyCode::F9: return ImGuiKey_F9;
        case KeyCode::F10: return ImGuiKey_F10;
        case KeyCode::F11: return ImGuiKey_F11;
        case KeyCode::F12: return ImGuiKey_F12;
        case KeyCode::F13: return ImGuiKey_F13;
        case KeyCode::F14: return ImGuiKey_F14;
        case KeyCode::F15: return ImGuiKey_F15;
        case KeyCode::F16: return ImGuiKey_F16;
        case KeyCode::F17: return ImGuiKey_F17;
        case KeyCode::F18: return ImGuiKey_F18;
        case KeyCode::F19: return ImGuiKey_F19;
        case KeyCode::F20: return ImGuiKey_F20;
        case KeyCode::F21: return ImGuiKey_F21;
        case KeyCode::F22: return ImGuiKey_F22;
        case KeyCode::F23: return ImGuiKey_F23;
        case KeyCode::F24: return ImGuiKey_F24;
        default: return ImGuiKey_None;
    }
}

void ImGuiLayer::OnAttach()
{
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        // Viewports do not work on wayland, because it doesnt allow windows
        // changing its own position. This enables it on windows/x11.
        TRY_ENABLE_VIEWPORTS

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        auto window = SCAST<GLFWwindow*>(
                Application::GetWindow().WindowImpl());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        const AbstractWindow& wn = Application::GetWindow();
        io.DisplaySize = ImVec2(wn.GetWidth(), wn.GetHeight());
        SetDefaultColors();
}

void ImGuiLayer::OnDetach()
{
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
}

void ImGuiLayer::RenderPrepare()
{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
}

void ImGuiLayer::RenderFinish()
{
        ImGuiIO& io = ImGui::GetIO();
        auto& window = Application::GetWindow();
        io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                GLFWwindow* context_backup = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(context_backup);
        }
}

void ImGuiLayer::OnImGuiRender()
{
}

void ImGuiLayer::OnEvent(Event& e)
{
        if (!m_BlockEvents)
                return;

        ImGuiIO& io = ImGui::GetIO();
        if ((e.IncludesCategory(EventCategory::kKeyboard) && io.WantCaptureKeyboard) ||
            (e.IncludesCategory(EventCategory::kMouse) && io.WantCaptureMouse))
                e.Handle();
}

constexpr ImVec4 ToImVec4(const glm::vec4& v)
{
        return ImVec4{v.r, v.g, v.b, v.a};
}

void ImGuiLayer::SetDefaultColors()
{
        auto& colors = ImGui::GetStyle().Colors;
        auto c1 = ToImVec4(Colors::FROM_RGB(224, 164, 6));
        auto c2 = ToImVec4(Colors::FROM_RGB(251, 206, 7));
        auto c3 = ToImVec4(Colors::FROM_RGB(89, 64, 30));
        auto c4 = ToImVec4(Colors::FROM_RGB(41, 38, 25));
        auto c5 = ToImVec4(Colors::FROM_RGB(193, 178, 148));

        colors[ImGuiCol_WindowBg] = c4;

        colors[ImGuiCol_Header] = c1;
        colors[ImGuiCol_HeaderHovered] = c2;
        colors[ImGuiCol_HeaderActive] = c3;

        colors[ImGuiCol_Button] = c1;
        colors[ImGuiCol_ButtonHovered] = c2;
        colors[ImGuiCol_ButtonActive] = c3;

        colors[ImGuiCol_FrameBg] = c1;
        colors[ImGuiCol_FrameBgHovered] = c2;
        colors[ImGuiCol_FrameBgActive] = c3;

        colors[ImGuiCol_Tab] = c1;
        colors[ImGuiCol_TabHovered] = c2;
        colors[ImGuiCol_TabActive] = c3;
        colors[ImGuiCol_TabUnfocused] = c4;
        colors[ImGuiCol_TabUnfocusedActive] = c1;

        colors[ImGuiCol_TitleBg] = c1;
        colors[ImGuiCol_TitleBgActive] = c1;
        colors[ImGuiCol_TitleBgCollapsed] = c5;

}


}  // namespace Borek
