// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "miniaudio.h"
#include <string>

namespace Borek {

class SoundEngine {
public:
        static void
        Init();

        static void
        Deinitialize();

        static void
        Play(const std::string& path);

        static ma_engine&
        Get() { return s_Engine; }

        static bool
        IsSoundValid(const struct SoundAsset& sound);

        static int
        GetGeneration() { return s_EngineGeneration; };

private:
        static ma_engine s_Engine;
        static bool s_EngineInitialized;
        static int s_EngineGeneration;
};

}  // namespace Borek
