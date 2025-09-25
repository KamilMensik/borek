// Copyright 2024-2025 <kamilekmensik@gmail.com>


#pragma once

#include <functional>

namespace Borek {

struct PairHash {
    template <class T1, class T2>
    size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
    }
};

}  // namespace Borek
