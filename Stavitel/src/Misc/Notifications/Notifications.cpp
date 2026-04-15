// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Misc/Notifications/Notifications.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include "Misc/FontAwesome.h"
#include "glm/ext/vector_float2.hpp"
#include "imgui.h"

namespace Borek {

const ImGuiWindowFlags flags =
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoDocking;

const Map<NotificationType_, std::string_view> type_default_name = {
        { NotificationType_None, "" },
        { NotificationType_Success, "Success" },
        { NotificationType_Error, "Error" },
        { NotificationType_Info, "Info" },
        { NotificationType_Warning, "Warning" },
};

const Map<NotificationType_, std::pair<std::string_view, ImVec4>> type_icon = {
        { NotificationType_None,
          { "", { 255, 255, 255, 255 } } },
        { NotificationType_Success,
          { ICON_FA_CIRCLE_CHECK, { 0, 255, 0, 255 } } },
        { NotificationType_Error,
          { ICON_FA_CIRCLE_EXCLAMATION, { 255, 0, 0, 255 } } },
        { NotificationType_Info,
          { ICON_FA_CIRCLE_INFO, { 0, 157, 255, 255 } } },
        { NotificationType_Warning,
          { ICON_FA_TRIANGLE_EXCLAMATION, { 0, 255, 0, 255 } } },
};

const glm::vec2 padding(10.0f, 10.0f);

void
Notifications::Render(float delta)
{
        auto& io = ImGui::GetIO();
        ImGui::PushFont(io.Fonts->Fonts[2]);

        const ImVec2 vpsize = ImGui::GetMainViewport()->Size;
        float height = 0.0f;

        ImGui::PushID("Notifications");

        char window_name[50]{};
        for (int i = 0; auto& it : s_NotificationQueue) {
                snprintf(window_name, sizeof(window_name), "##NOTIF%d", i);
                ImGui::SetNextWindowPos(ImVec2(vpsize.x - padding.x, vpsize.y - height - padding.y),
                                        ImGuiCond_Always, ImVec2(1.0f, 1.0f));
                ImGui::Begin(window_name, nullptr, flags);

                ImGui::PushTextWrapPos(vpsize.x / 3.f);

                auto& icn = type_icon.find(it.type)->second;
                if (!icn.first.empty()) {
                        ImGui::TextColored(icn.second, "%s", icn.first.data());
                }
                std::string_view title;
                title = it.title.empty() ? type_default_name.find(it.type)->second : it.title;
                if (!title.empty()) {
                        if (!icn.first.empty())
                                ImGui::SameLine();

                        ImGui::Text("%s", title.data());
                }

                if (!it.text.empty()) {
                        if (!(title.empty() && icn.first.empty()))
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

                        ImGui::Text("%s", it.text.data());
                }

                it.duration -= delta;
                height += ImGui::GetWindowHeight() + padding.y;

                ImGui::PopTextWrapPos();
                ImGui::End();
                i++;
        }

        for (int i = s_NotificationQueue.size() - 1; i >= 0; i--) {
                auto it = s_NotificationQueue.begin() + i;
                if (it->duration <= 0)
                        s_NotificationQueue.erase(it);
        }

        ImGui::PopID();
        ImGui::PopFont();
}

std::vector<Notification> Notifications::s_NotificationQueue;

}  // namespace Borek
