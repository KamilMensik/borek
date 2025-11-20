// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <vector>
#include <functional>

#define _BASE_EVENT_HEADER(_cls)                                        \
public:                                                                 \
        virtual void                                                    \
        Notify();                                                       \
                                                                        \
        static EVHandle                                                 \
        AddListener(const std::function<void(_cls&)> callback);         \
                                                                        \
        static void                                                     \
        RemoveListener(EVHandle handle);                                \
                                                                        \
        static void                                                     \
        Grab(EVHandle handle);                                          \
                                                                        \
        static void                                                     \
        Release(EVHandle handle);                                       \
                                                                        \
private:                                                                \
        static std::vector<std::function<void(_cls&)>> s_Listeners;     \
        static std::vector<std::pair<uint32_t, uint32_t>> s_StableHandles;\
        static uint32_t s_FirstEmptySpace;                              \
        static EVHandle s_BlockingListener;                             \

#define _BASE_EVENT_IMPL(_cls, _parent)                                 \
        void _cls::Notify()                                             \
        {                                                               \
                if (s_BlockingListener != UINT32_MAX) {                 \
                        auto& sh = s_StableHandles[s_BlockingListener]; \
                        s_Listeners[sh.first](*this);                   \
                        return;                                         \
               }                                                        \
                                                                        \
                for (auto& listener : s_Listeners) {                    \
                        listener(*this);                                \
                }                                                       \
                                                                        \
                _parent::Notify();                                      \
        }                                                               \
                                                                        \
                                                                        \
        EVHandle _cls::AddListener(const std::function<void(_cls&)> cb) \
        {                                                               \
                EVHandle handle;                                        \
                if (s_FirstEmptySpace != UINT32_MAX) {                  \
                        handle = s_FirstEmptySpace;                     \
                        s_FirstEmptySpace = s_StableHandles[s_FirstEmptySpace].first; \
                } else {                                                \
                        handle = s_StableHandles.size();                \
                        s_StableHandles.emplace_back();                 \
                }                                                       \
                                                                        \
                s_StableHandles[handle].first = s_Listeners.size();     \
                s_StableHandles[s_Listeners.size()].second = handle;    \
                s_Listeners.emplace_back(cb);                           \
                return handle;                                          \
        }                                                               \
                                                                        \
        void _cls::RemoveListener(EVHandle handle)                      \
        {                                                               \
                uint32_t listener_index = s_StableHandles[handle].first;\
                s_Listeners[listener_index] = s_Listeners.back();       \
                s_Listeners.pop_back();                                 \
                                                                        \
                uint32_t moved_index = s_StableHandles[s_Listeners.size()].second; \
                s_StableHandles[moved_index].first = listener_index;    \
                s_StableHandles[listener_index].second = moved_index;   \
                                                                        \
                s_StableHandles[handle].first = s_FirstEmptySpace;      \
                s_FirstEmptySpace = handle;                             \
        }                                                               \
                                                                        \
        void _cls::Grab(EVHandle handle)                                \
        {                                                               \
                if (s_BlockingListener != UINT32_MAX)                   \
                        return;                                         \
                                                                        \
                s_BlockingListener = handle;                            \
        }                                                               \
                                                                        \
        void _cls::Release(EVHandle handle)                             \
        {                                                               \
                if (s_BlockingListener != handle)                       \
                        return;                                         \
                                                                        \
                s_BlockingListener = UINT32_MAX;                        \
        }                                                               \
                                                                        \
        std::vector<std::function<void(_cls&)>> _cls::s_Listeners;      \
        std::vector<std::pair<uint32_t, uint32_t>> _cls::s_StableHandles;\
        uint32_t _cls::s_FirstEmptySpace = UINT32_MAX;                  \
        uint32_t _cls::s_BlockingListener = UINT32_MAX;

namespace Borek {

using EVHandle = uint32_t;

class IEvent {
public:
        void Notify() {};
};

class Event : public IEvent {
        _BASE_EVENT_HEADER(Event)
};

}  // namespace Borek
