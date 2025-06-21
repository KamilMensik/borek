// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>

namespace ECS {

struct ComponentColumn {
        void* data;
        uint32_t element_size;
        uint32_t alignment;
        void (*constructor)(void*);
        void (*destructor)(void*);
        std::string (*to_s_internal)(void*);
        ComponentColumn(uint32_t element_size, uint32_t alignment,
                        void(*constructor)(void*), void(*destructor)(void*),
                        std::string(*to_s_internal)(void*))
                : element_size(element_size), alignment(alignment),
                  constructor(constructor), destructor(destructor),
                  to_s_internal(to_s_internal) {}

        inline void* operator [](uint32_t index)
        {
                return (uint8_t*)data + (element_size * index);
        }

        std::string to_s(int until) const;
};

}  // namespace ECS
