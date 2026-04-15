// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>
#include <string_view>

namespace Borek {

enum NotificationType_ {
        NotificationType_None,
        NotificationType_Success,
        NotificationType_Warning,
        NotificationType_Info,
        NotificationType_Error,
};

struct Notification {
        Notification() = default;
        Notification(std::string_view text) : text(text) {}

        inline Notification&
        Text(std::string_view val)
        {
                text = val;
                return *this;
        }

        inline Notification&
        Title(std::string_view val)
        {
                title = val;
                return *this;
        }

        inline Notification&
        Type(NotificationType_ val)
        {
                type = val;
                return *this;
        }

        inline Notification&
        Duration(float val)
        {
                title = val;
                return *this;
        }

        std::string text;
        std::string title;
        float duration = 5.0f;
        NotificationType_ type = NotificationType_None;
};

}  // namespace Borek
