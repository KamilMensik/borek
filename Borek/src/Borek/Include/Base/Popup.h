// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

namespace Borek {

class Popup {
public:
        // returns true if is handled
        virtual bool Tick();
        virtual ~Popup() = default;

protected:
        bool m_Initialized = false;
};

}  // namespace Borek
