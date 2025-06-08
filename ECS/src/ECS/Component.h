// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
namespace ECS {

template <class T>
struct Component {
        static uint32_t component_id;
        static uint32_t Id() { return component_id; }
};

template <class T>
uint32_t Component<T>::component_id = UINT32_MAX;

struct ComponentData {
        uint32_t size;
        uint32_t alignment;
        void (*constructor)(void*);
        void (*destructor)(void*);
};

}  // namespace ECS
