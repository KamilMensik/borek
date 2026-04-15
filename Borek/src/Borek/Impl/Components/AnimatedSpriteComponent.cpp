// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cmath>

#include "Include/Components/AnimatedSpriteComponent.h"

namespace Borek {

void
AnimatedSpriteComponent::Step(Time new_frame)
{
        if (!flags.HasFlags(AnimatedSpriteComponentFlags_Playing))
                return;

        const uint32_t anim_index = anim->animation_by_name[current_animation];
        const uint32_t fps = anim->animation_frames[anim_index];
        const float diff = new_frame.Seconds() - prev_frame.Seconds();

        if (diff >= 1.0f / fps) {
                float integral;
                std::modf(diff * fps, &integral);
                prev_frame = prev_frame.Seconds() + integral / fps;

                if (anim->animation_frames[++current_frame] == UINT32_MAX) {
                        current_frame = anim_index + 1;
                }
        }
}

bool
AnimatedSpriteComponent::ChangeAnimation(Symbol anim)
{
        auto it = this->anim->animation_by_name.find(anim);
        if (it == this->anim->animation_by_name.end())
                return false;

        if (current_animation.Value() == anim.Value())
                return true;

        current_animation = anim;
        current_frame = it->second + 1;
        return true;
}

}  // namespace Borek
