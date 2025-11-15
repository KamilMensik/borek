// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <vector>

namespace ECS {

struct ComponentData {
        uint32_t size;
        uint32_t alignment;
        void (*constructor)(void*);
        void (*destructor)(void*);
        void (*moveop)(void*, void*);
};

extern std::vector<ComponentData> s_ComponentData;

template <class T>
constexpr void RegisterComponent()
{
        void (*constructor)(void*) = [](void* address){
                new(address) T();
        };
        void (*destructor)(void*) = [](void* address){
                reinterpret_cast<T*>(address)->~T();
        };
        void (*moveop)(void*, void*) = [](void* dest, void* src) {
                new(dest) T(std::move(*reinterpret_cast<T*>(src)));
        };
        s_ComponentData.emplace_back(sizeof(T), alignof(T), constructor,
                                     destructor, moveop);
}


extern uint32_t s_ComponentCounter;
template <class T>
constexpr uint32_t GetId()
{
        static uint32_t s_ComponentId = s_ComponentCounter++;

        if (s_ComponentData.size() <= s_ComponentId) {
                RegisterComponent<T>();
        }

        return s_ComponentId;
}

}  // namespace ECS
