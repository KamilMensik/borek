// Copyright 2024-2026 <kamilekmensik@gmail.com>

#include "Include/Base/TransformCache.h"

namespace Borek {

void
TransformCache::Reset()
{
        s_Map.clear();
}

const TransformComponent&
TransformCache::GetTransform(Entity e)
{
        auto it = s_Map.find(e);
        if (it != s_Map.end() && it->second.first)
                return it->second.second;

        s_Map[e].first = true;
        s_Map[e].second = e.Transform();

        Entity parent = e.GetParent();
        if (!parent.IsNil())
                s_Map[e].second += GetTransform(e.GetParent());

        return s_Map[e].second;
}

void
TransformCache::Invalidate(Entity e)
{
        auto it = s_Map.find(e);
        if (it == s_Map.end() || !it->second.first)
                return;

        it->second.first = false;
        auto children = e.GetChildren();
        if (!children) return;

        for (Entity child : *children)
                Invalidate(child);
}

std::unordered_map<uint32_t, std::pair<bool, TransformComponent>> TransformCache::s_Map;

}  // namespace Borek
