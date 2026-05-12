// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Popup.h"

namespace Borek {
namespace Popups {

class ExportPopup : public Popup {
public:
        ExportPopup() = default;

        bool Tick() override;
};

}  // namespace Popups
}  // namespace Borek
