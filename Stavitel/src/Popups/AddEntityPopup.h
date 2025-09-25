// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>

#include "Include/Base/Popup.h"

namespace Borek {
namespace Popups {

class AddEntityPopup : public Popup {
public:
        AddEntityPopup(uint32_t parent_id);

        bool Tick() override;

private:
        int m_SelectedItem = 0;
        std::string m_Search = "";
        std::string m_NodeName = "";
        uint32_t m_ParentId;
};

}  // namespace Popups
}  // namespace Borek
