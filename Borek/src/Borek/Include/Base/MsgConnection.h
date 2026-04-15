// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

namespace Borek {

struct MsgConnection {
        MsgConnection() : rbsym(UINT32_MAX), entity_id(UINT32_MAX) { }
        MsgConnection(uint32_t rbsym, uint32_t entity_id)
                : rbsym(rbsym), entity_id(entity_id) {}

        constexpr inline bool
        IsNil() { return rbsym == UINT32_MAX; }

        void
        Call(uint64_t args);

        bool
        operator ==(const MsgConnection& other) const;

        uint32_t rbsym;
        uint32_t entity_id;
};

}  // namespace Borek
