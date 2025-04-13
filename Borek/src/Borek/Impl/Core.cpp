// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"

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
        }

        return 0;
}

}  // namespace Borek
