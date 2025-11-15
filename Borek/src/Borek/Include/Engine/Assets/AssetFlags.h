// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

namespace Borek {

enum TexFlags {
        TexFlags_Filtering = 0b1111,
        TexFlags_Nearest = 0,
        TexFlags_Linear = 1,
};

enum SoundFlags {
        SoundFlags_Looping = 1,
        SoundFlags_Stream = 2,
};

}  // namespace Borek
