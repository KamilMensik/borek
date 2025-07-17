// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "ECS/Query.h"

namespace Borek {

template <typename ... components>
using Query = ECS::Query<components...>;

}  // namespace Borek
