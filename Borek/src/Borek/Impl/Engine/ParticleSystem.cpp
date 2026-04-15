// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <format>
#include <ranges>

#include "Include/Engine/ParticleSystem.h"
#include "Include/Drawing/Quad.h"
#include "Include/Base/Random.h"

namespace Borek {

template <class T>
inline static T
lerp(const T& from, const T& to, float i)
{
        T res = from;
        res *= i;
        res += to * (1 - i);
        return res;
}

void
ParticleSystem::OnUpdate(float delta)
{
        for (int i = 0; i < s_Pool.size(); i++) {
                Particle& p = s_Pool[i];
                if (p.lifetime_remaining <= 0) {
                        s_Pool[i] = s_Pool.back();
                        s_Pool.pop_back();
                }

                p.lifetime_remaining -= delta;
                p.position += p.velocity * delta;
                p.rotation += 0.01f * delta;
        }
}

void
ParticleSystem::Render()
{
        for (const Particle& p : std::ranges::views::reverse(s_Pool)) {
                float lerp_i = p.lifetime_remaining / p.lifetime;
                Drawing::Quad::Draw(p.position,
                                    glm::vec2(lerp(p.from_size, p.to_size, lerp_i)),
                                    p.rotation,
                                    lerp(p.from_color, p.to_color, lerp_i),
                                    p.zindex);
        }
}

void
ParticleSystem::Emit(const ParticleParams& params, const glm::vec2& position,
                     float zindex)
{
        Particle& p = s_Pool.emplace_back(
                position, params.velocity, params.from_color, params.to_color,
                Random::Float(360), params.from_size, params.to_size,
                params.lifetime, params.lifetime, zindex);

        p.velocity.x += params.velocity_variation.x * (Random::Float() - 0.5f);
        p.velocity.y += params.velocity_variation.y * (Random::Float() - 0.5f);
}

std::vector<Particle> ParticleSystem::s_Pool;
}  // namespace Borek
