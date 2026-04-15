// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Debug/Log.h"
#include <algorithm>
#include <variant>
#include <vector>
#include <array>
#include <cstdint>

namespace Borek {

template <class T, size_t stack_limit>
struct SmallVecSet {
        using values_on_heap = std::vector<T>;
        struct values_on_stack {
                std::array<T, stack_limit> values;
                uint32_t size;
        };


        SmallVecSet() : m_Values(values_on_stack({}, 0)) {}

        inline T*
        begin()
        {
                if (std::holds_alternative<values_on_heap>(m_Values)) {
                        return &std::get<values_on_heap>(m_Values).front();
                } else {
                        return &std::get<values_on_stack>(m_Values).values.front();
                }
        }

        inline T*
        end()
        {
                if (std::holds_alternative<values_on_heap>(m_Values)) {
                        return &(&std::get<values_on_heap>(m_Values).back())[1];
                } else {
                        auto& vos = std::get<values_on_stack>(m_Values);
                        return &(&vos.values.front())[vos.size];
                }
        }

        inline bool
        empty()
        {
                if (std::holds_alternative<values_on_heap>(m_Values)) {
                        return std::get<values_on_heap>(m_Values).empty();
                } else {
                        return std::get<values_on_stack>(m_Values).size == 0;
                }
        }

        inline size_t
        size()
        {
                if (std::holds_alternative<values_on_heap>(m_Values)) {
                        return std::get<values_on_heap>(m_Values).size();
                } else {
                        return std::get<values_on_stack>(m_Values).size;
                }
        }

        inline T*
        get(const T& item)
        {
                auto it = std::find(begin(), end(), item);
                if (it == end())
                        return nullptr;

                return it;
        }

        void
        erase(const T& item)
        {
                auto it = std::remove(begin(), end(), item);
                if (it == end())
                        return;

                if (std::holds_alternative<values_on_heap>(m_Values)) {
                        std::get<values_on_heap>(m_Values).pop_back();
                } else {
                        auto& vos = std::get<values_on_stack>(m_Values);
                        vos.values[vos.size--].~T();
                }
        }

        void
        insert(const T& item)
        {
                if (std::find(begin(), end(), item) == end())
                        add(item);
        }

        void
        insert(T&& item)
        {
                if (std::find(begin(), end(), item) == end())
                        add(item);
        }

        bool
        contains(const T& item)
        {
                return std::find(begin(), end(), item) != end();
        }

private:
        std::variant<values_on_stack, values_on_heap> m_Values;

        inline void
        add(const T& item)
        {
                if (std::holds_alternative<values_on_heap>(m_Values)) {
                        std::get<values_on_heap>(m_Values).emplace_back(item);
                } else {
                        auto& vos = std::get<values_on_stack>(m_Values);
                        if (vos.size < stack_limit) {
                                vos.values[vos.size++] = item;
                                return;
                        }

                        values_on_heap new_vals(vos.values.begin(), vos.values.end());
                        new_vals.emplace_back(item);
                        m_Values = std::move(new_vals);
                }
        }

        inline void
        add(T&& item)
        {
                if (std::holds_alternative<values_on_heap>(m_Values)) {
                        std::get<values_on_heap>(m_Values).emplace_back(item);
                } else {
                        auto& vos = std::get<values_on_stack>(m_Values);
                        if (vos.size < stack_limit) {
                                vos.values[vos.size++] = item;
                                return;
                        }

                        values_on_heap new_vals(vos.values.begin(), vos.values.end());
                        new_vals.emplace_back(item);
                        m_Values = std::move(new_vals);
                }
        }
};

}  // namespace Borek
