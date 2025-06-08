// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "ECS/Component.h"
#include <concepts>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace ECS {

struct View {
        View(int component_size) : data(component_size) {}
        template <class T>
        inline T& get(uint32_t index)
        {
                return *reinterpret_cast<T*>(get(index));
        }
        template <class T> requires std::derived_from<T, Component<T>>
        inline T& get()
        {
                return *reinterpret_cast<T*>(get(c_index[T::component_id]));
        }
        inline void* get(uint32_t index)
        {
                return data[index];
        }

        uint32_t current_entity_id;
        std::vector<void*> data;
        std::unordered_map<uint32_t, uint32_t> c_index;
};

}  // namespace ECS
