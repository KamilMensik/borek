// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Sound.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/SoundAsset.h"
#include "miniaudio.h"

namespace Borek {

void
SoundEngine::Init()
{
        ma_result result;
        result = ma_engine_init(NULL, &s_Engine);
        if (result != MA_SUCCESS) {
                BOREK_ENGINE_ERROR("Failed to initialize sound engine, code: {}",
                                   (int)result);
                return;
        }

        BOREK_ENGINE_INFO("INITIALIZED SOUND ENGINE");
        s_EngineInitialized = true;
        s_EngineGeneration++;
}

void
SoundEngine::Deinitialize()
{
        if (!s_EngineInitialized)
                return;

        ma_engine_uninit(&s_Engine);
        s_EngineInitialized = false;
}

void
SoundEngine::Play(const std::string& path)
{
        BOREK_ENGINE_INFO("Played sound, got result: {}", (int)ma_engine_play_sound(&s_Engine, path.c_str(), NULL));
}

bool
SoundEngine::IsSoundValid(const SoundAsset& sound)
{
        return s_EngineInitialized && sound.engine_generation == s_EngineGeneration;
}

ma_engine SoundEngine::s_Engine;
bool SoundEngine::s_EngineInitialized;
int SoundEngine::s_EngineGeneration;


}  // namespace Borek
