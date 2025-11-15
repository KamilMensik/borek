// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include <cstdint>

namespace Borek {

uint32_t DatatypeSize(Datatype d)
{
        switch (d) {
        case Datatype::Float:
                return sizeof(float);
        case Datatype::Float2:
                return sizeof(float) * 2;
        case Datatype::Float3:
                return sizeof(float) * 3;
        case Datatype::Float4:
                return sizeof(float) * 4;
        case Datatype::Int:
                return sizeof(int);
        case Datatype::Int2:
                return sizeof(int) * 2;
        case Datatype::Int3:
                return sizeof(int) * 3;
        case Datatype::Int4:
                return sizeof(int) * 4;
        case Datatype::Sampler2D:
                return sizeof(uint64_t);
        }

        return 0;
}

}  // namespace Borek
