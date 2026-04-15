// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Assets/AnimationAsset.h"
#include "Include/Core.h"

#include <cstdint>
#include <string>

namespace Borek {
namespace Panels {

class AnimationPanel {
public:
        AnimationPanel();
        ~AnimationPanel();
        void OnImguiRender();

private:
        Uniq<AnimationAsset> m_Asset;
        std::string m_AssetPath;
        std::string m_CurrentAnimationName;
        std::vector<uint32_t> m_CurrentAnimation;
        Time m_PrevTime;
        uint32_t m_CurrentAnimationFrame;
        bool m_IsPlayingAnim;

        void
        DrawAnimFileInput();

        void
        DrawSpritesheetInput();

        void
        DrawAnimation();

        void
        DrawSelectAnimation();

        void
        DeleteAnim(const std::string& name);

        void
        SaveAnim();
};

}  // namespace Panels
}  // namespace Borek
