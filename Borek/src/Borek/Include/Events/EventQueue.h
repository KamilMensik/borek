// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include <concepts>
#include <cstdint>
#include <vector>

#include "Include/Events/Event.h"

namespace Borek {

struct EventQueueItem {
        uint8_t* data;
        uint32_t item_size;
        uint32_t cur;
        uint32_t max_size;
        void (*move_op)(void* dest, void* src);

        EventQueueItem() = default;
        EventQueueItem(uint32_t item_size, void(*move_op)(void*,void*));

        void
        Grow();

        void*
        Get(uint32_t index);

        void
        Clean();
};

class EventQueue {
public:
        EventQueue();
        ~EventQueue();

        template <class T, typename ... Args> requires std::derived_from<T, Event>
        void
        Add(Args...args)
        {
                const uint32_t event_id = GetEventId<T>();

                if (m_Items->size() <= event_id) {
                        auto move_op = [](void* dest, void* src) {
                                new(dest) T(std::move(*RCAST<T*>(src)));
                        };
                        m_ItemsA.emplace_back(sizeof(T), move_op);
                        m_ItemsB.emplace_back(sizeof(T), move_op);
                }

                EventQueueItem& eqm = m_Items->at(event_id);
                m_Queue->emplace_back(event_id, eqm.cur);
                new(eqm.Get(eqm.cur)) T(std::forward<Args>(args)...);
                eqm.cur++;
        }

        void
        Handle();

private:
        std::vector<EventQueueItem> m_ItemsA;
        std::vector<EventQueueItem> m_ItemsB;
        std::vector<EventQueueItem>* m_Items;
        std::vector<bool> m_BlockedEvents;
        std::vector<std::pair<uint32_t, uint32_t>> m_QueueA;
        std::vector<std::pair<uint32_t, uint32_t>> m_QueueB;
        std::vector<std::pair<uint32_t, uint32_t>>* m_Queue;

        static uint32_t s_CurrentEventId;

        template <class T> requires std::derived_from<T, Event>
        static uint32_t
        GetEventId()
        {
                static uint32_t id = s_CurrentEventId++;
                return id;
        }
};

}  // namespace Borek
