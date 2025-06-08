// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/Event.h"
#include <cstdint>

namespace Borek {

constexpr static uint32_t GenerateNewCustomEventId() {
        static uint32_t current = 0;
        return current++;
}

template <class T>
class CustomEvent : public Event {
public:
        static inline uint32_t GetCustomEventId() { return s_CustomEventId; }
        inline uint32_t GetEventId() { return m_CustomEventId; }
        virtual const EventType GetEventType() const override
        {
                return EventType::kCustomEvent;
        }
        virtual const int GetEventCategories() const override
        {
                return EventCategory::kCustom;
        }
        virtual const char *GetName() const override
        {
                return "Custom Event";
        }
        CustomEvent() { m_CustomEventId = s_CustomEventId; }
private:
        static const uint32_t s_CustomEventId;
        uint32_t m_CustomEventId;
};

template <class T>
const uint32_t CustomEvent<T>::s_CustomEventId = GenerateNewCustomEventId();

}  // namespace Borek
