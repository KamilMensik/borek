// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Application.h"
#include "Include/Base/Colors.h"
#include "Include/Base/Symbol.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/AnimationAsset.h"

namespace Borek {

enum AnimatedSpriteComponentFlags {
        AnimatedSpriteComponentFlags_Flipping = 1 | 2,
        AnimatedSpriteComponentFlags_FlipX = 1,
        AnimatedSpriteComponentFlags_FlipY = 2,
        AnimatedSpriteComponentFlags_Repeat = 4,
        AnimatedSpriteComponentFlags_Playing = 8,
};

struct AnimatedSpriteComponent {
        Asset<AnimationAsset> anim;
        Color color;
        BitFlags flags;
        uint16_t size_x = 32, size_y = 32;
        uint32_t current_frame = 0;
        Symbol current_animation;
        Time prev_frame;

        AnimatedSpriteComponent()
                : color(Colors::WHITE), prev_frame(Application::GetTime())
        {}

        AnimatedSpriteComponent(Asset<AnimationAsset>& anim,
                                Color color = Colors::WHITE)
                : anim(anim), color(color), prev_frame(Application::GetTime())
        {}

        void
        Step(Time new_frame);

        bool
        ChangeAnimation(Symbol anim);
};


}  // namespace Borek
