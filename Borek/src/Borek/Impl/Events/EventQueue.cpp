// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Events/EventQueue.h"
#include "Include/Core.h"
#include "Include/Debug/Log.h"
#include <cstdint>
#include <cstring>

namespace Borek {

EventQueueItem::EventQueueItem(uint32_t item_size, void(*move_op)(void*, void*))
{
        data = new uint8_t[item_size * 5];
        this->item_size = item_size;
        cur = 0;
        max_size = 5;
        this->move_op = move_op;
}

void
EventQueueItem::Grow()
{
        max_size = max_size * 2;
        uint8_t* new_data = new uint8_t[max_size * item_size];

        for (uint32_t i = 0; i < cur; i++) {
                const uint32_t offset = i * item_size;
                move_op(new_data + offset, data + offset);
        }

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

void
EventQueueItem::Clean()
{
        for (uint32_t i = 0; i < cur; i++) {
                const uint32_t offset = i * item_size;
                RCAST<Event*>(data + offset)->~Event();
        }
}

EventQueue::EventQueue()
{
        m_Queue = &m_QueueA;
        m_Items = &m_ItemsA;
}

EventQueue::~EventQueue()
{
        for (auto& item : m_ItemsA) {
                item.Clean();
                delete[] item.data;
        }
        for (auto& item : m_ItemsB) {
                item.Clean();
                delete[] item.data;
        }
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
                e->~Event();
        }

        for (EventQueueItem& eqm : items) {
                eqm.cur = 0;
        }

        queue.clear();
}

uint32_t EventQueue::s_CurrentEventId = 0;

}  // namespace Borek
