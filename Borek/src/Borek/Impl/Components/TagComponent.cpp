// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/TagComponent.h"

namespace Borek {

TagComponent::TagComponent()
{
}

void
TagComponent::AddTag(const Symbol& tag)
{
        insert(tag);
}

void
TagComponent::RemoveTag(const Symbol& tag)
{
        erase(tag);
}

bool
TagComponent::Contains(const Symbol& sym)
{
        return contains(sym);
}


}  // namespace Borek
