// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <iostream>

namespace ECS {

struct ComponentColumn {
        void* data;
        uint32_t element_size;
        uint32_t alignment;
        void (*constructor)(void*);
        void (*destructor)(void*);
        ComponentColumn(uint32_t element_size, uint32_t alignment,
                        void(*constructor)(void*), void(*destructor)(void*))
                : element_size(element_size), alignment(alignment),
                  constructor(constructor), destructor(destructor) {}

        inline void* operator [](uint32_t index)
        {
                return (uint8_t*)data + (element_size * index);
        }

        std::string to_s(int until) const;
};

}  // namespace ECS
