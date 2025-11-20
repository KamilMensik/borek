// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Events/EventQueue.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include <cstdint>
#include <cstring>

namespace Borek {

EventQueueItem::EventQueueItem(uint32_t item_size)
{
        data = new uint8_t[item_size * 5];
        this->item_size = item_size;
        cur = 0;
        max_size = 5;
}

void
EventQueueItem::Grow()
{
        BOREK_ENGINE_INFO("Growing event queue from {}, to {}", max_size, max_size * 2);
        max_size = max_size * 2;
        uint8_t* new_data = new uint8_t[max_size * item_size];

        std::memcpy(new_data, data, cur * item_size);

        delete[] data;
        data = new_data;
}

void*
EventQueueItem::Get(uint32_t index)
{
        while (index >= max_size) {
                Grow();
        }
        return &(data[item_size * index]);
}

EventQueue::EventQueue()
{
        m_Queue = &m_QueueA;
        m_Items = &m_ItemsA;
}

void
EventQueue::Handle()
{
        auto& queue = *m_Queue;
        auto& items = *m_Items;
        m_Queue = (m_Queue == &m_QueueA) ? &m_QueueB : &m_QueueA;
        m_Items = (m_Items == &m_ItemsA) ? &m_ItemsB : &m_ItemsA;

        for (auto [eqm_id, index] : queue) {
                Event* e = RCAST<Event*>(items[eqm_id].Get(index));
                e->Notify();
        }

        for (EventQueueItem& eqm : items) {
                eqm.cur = 0;
        }

        queue.clear();
}

uint32_t EventQueue::s_CurrentEventId = 0;

}  // namespace Borek
