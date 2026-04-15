// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Utils/SmallVecSet.h"
#include "Include/Base/Symbol.h"

namespace Borek {

struct TagComponent : public SmallVecSet<Symbol, 5> {
        TagComponent();

        void
        AddTag(const Symbol& tag);

        void
        RemoveTag(const Symbol& tag);

        bool
        Contains(const Symbol& sym);
};

}  // namespace Borek
