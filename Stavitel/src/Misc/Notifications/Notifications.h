// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <vector>

#include "Misc/Notifications/Notification.h"

namespace Borek {

class Notifications {
public:
        inline static void
        Add(const Notification& notification)
        {
                s_NotificationQueue.emplace_back(notification);
        }

        static void
        Render(float delta);

private:
        static std::vector<Notification> s_NotificationQueue;
};

}  // namespace Borek
