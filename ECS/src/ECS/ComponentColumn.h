// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

namespace ECS {

struct ComponentColumn {
        void* data;
        uint32_t element_size;
        uint32_t alignment;
        void (*constructor)(void*);
        void (*destructor)(void*);
        void (*moveop)(void*, void*);
        ComponentColumn(uint32_t element_size, uint32_t alignment,
                        void(*constructor)(void*), void(*destructor)(void*),
                        void(*moveop)(void*, void*))
                : element_size(element_size), alignment(alignment),
                  constructor(constructor), destructor(destructor),
                  moveop(moveop) {}

        void* operator [](uint32_t index);
};

}  // namespace ECS
