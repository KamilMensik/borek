#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Include/ImGui/ImGuiLayer.h"
#include "Include/ImGui/ImGuiImpl.h"
#include "Include/Application.h"
#include "Include/Input.h"

#ifdef WIN32
#define TRY_ENABLE_VIEWPORTS io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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
        case KeyCode::kTAB: return ImGuiKey_Tab;
        case KeyCode::kLEFT: return ImGuiKey_LeftArrow;
        case KeyCode::kRIGHT: return ImGuiKey_RightArrow;
        case KeyCode::kUP: return ImGuiKey_UpArrow;
        case KeyCode::kDOWN: return ImGuiKey_DownArrow;
        case KeyCode::kPAGE_UP: return ImGuiKey_PageUp;
        case KeyCode::kPAGE_DOWN: return ImGuiKey_PageDown;
        case KeyCode::kHOME: return ImGuiKey_Home;
        case KeyCode::kEND: return ImGuiKey_End;
        case KeyCode::kINSERT: return ImGuiKey_Insert;
        case KeyCode::kDELETE: return ImGuiKey_Delete;
        case KeyCode::kBACKSPACE: return ImGuiKey_Backspace;
        case KeyCode::kSPACE: return ImGuiKey_Space;
        case KeyCode::kENTER: return ImGuiKey_Enter;
        case KeyCode::kESCAPE: return ImGuiKey_Escape;
        case KeyCode::kAPOSTROPHE: return ImGuiKey_Apostrophe;
        case KeyCode::kCOMMA: return ImGuiKey_Comma;
        case KeyCode::kMINUS: return ImGuiKey_Minus;
        case KeyCode::kPERIOD: return ImGuiKey_Period;
        case KeyCode::kSLASH: return ImGuiKey_Slash;
        case KeyCode::kSEMICOLON: return ImGuiKey_Semicolon;
        case KeyCode::kEQUAL: return ImGuiKey_Equal;
        case KeyCode::kLEFT_BRACKET: return ImGuiKey_LeftBracket;
        case KeyCode::kBACKSLASH: return ImGuiKey_Backslash;
        case KeyCode::kWORLD_1: return ImGuiKey_Oem102;
        case KeyCode::kWORLD_2: return ImGuiKey_Oem102;
        case KeyCode::kRIGHT_BRACKET: return ImGuiKey_RightBracket;
        case KeyCode::kGRAVE_ACCENT: return ImGuiKey_GraveAccent;
        case KeyCode::kCAPS_LOCK: return ImGuiKey_CapsLock;
        case KeyCode::kSCROLL_LOCK: return ImGuiKey_ScrollLock;
        case KeyCode::kNUM_LOCK: return ImGuiKey_NumLock;
        case KeyCode::kPRINT_SCREEN: return ImGuiKey_PrintScreen;
        case KeyCode::kPAUSE: return ImGuiKey_Pause;
        case KeyCode::kKP_0: return ImGuiKey_Keypad0;
        case KeyCode::kKP_1: return ImGuiKey_Keypad1;
        case KeyCode::kKP_2: return ImGuiKey_Keypad2;
        case KeyCode::kKP_3: return ImGuiKey_Keypad3;
        case KeyCode::kKP_4: return ImGuiKey_Keypad4;
        case KeyCode::kKP_5: return ImGuiKey_Keypad5;
        case KeyCode::kKP_6: return ImGuiKey_Keypad6;
        case KeyCode::kKP_7: return ImGuiKey_Keypad7;
        case KeyCode::kKP_8: return ImGuiKey_Keypad8;
        case KeyCode::kKP_9: return ImGuiKey_Keypad9;
        case KeyCode::kKP_DECIMAL: return ImGuiKey_KeypadDecimal;
        case KeyCode::kKP_DIVIDE: return ImGuiKey_KeypadDivide;
        case KeyCode::kKP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case KeyCode::kKP_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case KeyCode::kKP_ADD: return ImGuiKey_KeypadAdd;
        case KeyCode::kKP_ENTER: return ImGuiKey_KeypadEnter;
        case KeyCode::kKP_EQUAL: return ImGuiKey_KeypadEqual;
        case KeyCode::kLEFT_SHIFT: return ImGuiKey_LeftShift;
        case KeyCode::kLEFT_CONTROL: return ImGuiKey_LeftCtrl;
        case KeyCode::kLEFT_ALT: return ImGuiKey_LeftAlt;
        case KeyCode::kLEFT_SUPER: return ImGuiKey_LeftSuper;
        case KeyCode::kRIGHT_SHIFT: return ImGuiKey_RightShift;
        case KeyCode::kRIGHT_CONTROL: return ImGuiKey_RightCtrl;
        case KeyCode::kRIGHT_ALT: return ImGuiKey_RightAlt;
        case KeyCode::kRIGHT_SUPER: return ImGuiKey_RightSuper;
        case KeyCode::kMENU: return ImGuiKey_Menu;
        case KeyCode::k0: return ImGuiKey_0;
        case KeyCode::k1: return ImGuiKey_1;
        case KeyCode::k2: return ImGuiKey_2;
        case KeyCode::k3: return ImGuiKey_3;
        case KeyCode::k4: return ImGuiKey_4;
        case KeyCode::k5: return ImGuiKey_5;
        case KeyCode::k6: return ImGuiKey_6;
        case KeyCode::k7: return ImGuiKey_7;
        case KeyCode::k8: return ImGuiKey_8;
        case KeyCode::k9: return ImGuiKey_9;
        case KeyCode::kA: return ImGuiKey_A;
        case KeyCode::kB: return ImGuiKey_B;
        case KeyCode::kC: return ImGuiKey_C;
        case KeyCode::kD: return ImGuiKey_D;
        case KeyCode::kE: return ImGuiKey_E;
        case KeyCode::kF: return ImGuiKey_F;
        case KeyCode::kG: return ImGuiKey_G;
        case KeyCode::kH: return ImGuiKey_H;
        case KeyCode::kI: return ImGuiKey_I;
        case KeyCode::kJ: return ImGuiKey_J;
        case KeyCode::kK: return ImGuiKey_K;
        case KeyCode::kL: return ImGuiKey_L;
        case KeyCode::kM: return ImGuiKey_M;
        case KeyCode::kN: return ImGuiKey_N;
        case KeyCode::kO: return ImGuiKey_O;
        case KeyCode::kP: return ImGuiKey_P;
        case KeyCode::kQ: return ImGuiKey_Q;
        case KeyCode::kR: return ImGuiKey_R;
        case KeyCode::kS: return ImGuiKey_S;
        case KeyCode::kT: return ImGuiKey_T;
        case KeyCode::kU: return ImGuiKey_U;
        case KeyCode::kV: return ImGuiKey_V;
        case KeyCode::kW: return ImGuiKey_W;
        case KeyCode::kX: return ImGuiKey_X;
        case KeyCode::kY: return ImGuiKey_Y;
        case KeyCode::kZ: return ImGuiKey_Z;
        case KeyCode::kF1: return ImGuiKey_F1;
        case KeyCode::kF2: return ImGuiKey_F2;
        case KeyCode::kF3: return ImGuiKey_F3;
        case KeyCode::kF4: return ImGuiKey_F4;
        case KeyCode::kF5: return ImGuiKey_F5;
        case KeyCode::kF6: return ImGuiKey_F6;
        case KeyCode::kF7: return ImGuiKey_F7;
        case KeyCode::kF8: return ImGuiKey_F8;
        case KeyCode::kF9: return ImGuiKey_F9;
        case KeyCode::kF10: return ImGuiKey_F10;
        case KeyCode::kF11: return ImGuiKey_F11;
        case KeyCode::kF12: return ImGuiKey_F12;
        case KeyCode::kF13: return ImGuiKey_F13;
        case KeyCode::kF14: return ImGuiKey_F14;
        case KeyCode::kF15: return ImGuiKey_F15;
        case KeyCode::kF16: return ImGuiKey_F16;
        case KeyCode::kF17: return ImGuiKey_F17;
        case KeyCode::kF18: return ImGuiKey_F18;
        case KeyCode::kF19: return ImGuiKey_F19;
        case KeyCode::kF20: return ImGuiKey_F20;
        case KeyCode::kF21: return ImGuiKey_F21;
        case KeyCode::kF22: return ImGuiKey_F22;
        case KeyCode::kF23: return ImGuiKey_F23;
        case KeyCode::kF24: return ImGuiKey_F24;
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
                Application::Instance().GetWindow().WindowImpl());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        const AbstractWindow& wn = Application::Instance().GetWindow();
        io.DisplaySize = ImVec2(wn.GetWidth(), wn.GetHeight());
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
}

void ImGuiLayer::RenderFinish()
{
        ImGuiIO& io = ImGui::GetIO();
        auto& window = Application::Instance().GetWindow();
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
        ImGui::ShowDemoWindow();
}



}
