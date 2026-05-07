// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <array>

#include "Include/Core.h"

namespace Borek {

// Basic Allocator for fast allocations
class Alloc {
public:
        template <size_t size>
        static uint64_t* Allocate()
        {
                static Ref<MemBlock> first_block;

                if (first_block == nullptr) {
                        first_block = NewRef<MemBlock>();
                        first_block->member_size = size;
                        first_block->size = 2024 / size;
                        MemBlock::register_range(first_block);
                }

                return first_block->alloc();
        }

        static void
        Free(uint64_t* mem);

private:
        struct MemBlock {
                std::array<uint64_t, 2024> mem;
                Ref<MemBlock> next = nullptr;

                uint16_t member_size;
                uint16_t size;
                uint16_t first_free = UINT16_MAX;
                uint16_t current_index = 0;
                uint16_t index;

                inline uint64_t*
                alloc() {
                        if (is_full()) {
                                if (next == nullptr) next = copy();

                                return next->alloc();
                        }

                        if (first_free != UINT16_MAX) {
                                uint64_t* res = &mem[first_free];
                                first_free = *res;
                                return res;
                        }

                        uint64_t* res = &mem[current_index * member_size];
                        ++current_index;
                        return res;
                }

                inline void
                free(uint64_t* mem)
                {
                        const uint64_t bgn = RCAST<uint64_t>(begin());
                        uint64_t index = (RCAST<uint64_t>(mem) - bgn) / 8;
                        this->mem[index] = first_free;
                        first_free = index;
                }

                inline constexpr uint64_t*
                begin() { return mem.begin(); }

                inline constexpr uint64_t*
                end() { return mem.end(); };

                inline constexpr bool
                is_full()
                {
                        return size == current_index && first_free == UINT16_MAX;
                }

                Ref<MemBlock>
                copy();

                static void
                register_range(Ref<MemBlock> block);
        };

        static std::vector<std::pair<uint64_t, Ref<MemBlock>>> s_MemBlocksBT;
};

template <class T>
class AUniq {
public:
        inline AUniq() : m_Ptr(nullptr) {}
        inline ~AUniq() { try_delete_ptr(); }
        inline AUniq(const AUniq&) = delete;
        inline AUniq& operator =(const AUniq&) = delete;
        inline AUniq(AUniq&& other)
        {
                m_Ptr = std::exchange(other.m_Ptr, nullptr);
        }
        inline AUniq& operator =(AUniq&& other) {
                try_delete_ptr();
                m_Ptr = std::exchange(other.m_Ptr, nullptr);
                return *this;
        }
        inline T& operator*() const { return *m_Ptr; }
        inline T* operator->() const { return m_Ptr; }
        inline T* get() const { return m_Ptr; }
        inline AUniq& reset(T* new_val)
        {
                try_delete_ptr();
                m_Ptr = new_val;
                return *this;
        }
        inline T* release() { return std::exchange(m_Ptr, nullptr); }
        inline AUniq& swap(AUniq& other)
        {
                m_Ptr = std::exchange(other.m_Ptr, m_Ptr);
        }
        inline operator bool() const { return m_Ptr != nullptr; }
        inline bool operator ==(const AUniq& other) const
        {
                return m_Ptr == other.m_Ptr;
        }


private:
        T* m_Ptr;

        inline void
        try_delete_ptr() {
                if (!m_Ptr) return;

                m_Ptr->~T();
                Alloc::Free(RCAST<uint64_t*>(m_Ptr));
        }
};

template <class T, class ... Args>
inline AUniq<T> NewAUniq(Args&&... args)
{
        constexpr size_t tsize = (sizeof(T) - 1) / 8 + 1;
        uint64_t* address = Alloc::Allocate<tsize>();
        return std::move(AUniq<T>().reset(new(address) T(std::forward<Args>(args)...)));
}

}  // namespace Borek
