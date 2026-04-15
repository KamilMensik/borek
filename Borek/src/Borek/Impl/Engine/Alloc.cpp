// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Alloc.h"

namespace Borek {

void
Alloc::Free(uint64_t* mem)
{
        const uint64_t val = RCAST<uint64_t>(mem);
        uint32_t l = 0;
        uint32_t r = s_MemBlocksBT.size();
        uint32_t c = (l + r) / 2;

        Ref<MemBlock> block;
        while (true) {
                uint64_t cur = s_MemBlocksBT[c].first;
                if (val < cur) {
                        r /= 2;
                        c = (l + r) / 2;
                } else if (val > (cur + 16192)) {
                        l = r / 2;
                        c = (l + r) / 2;
                } else {
                        block = s_MemBlocksBT[c].second;
                        break;
                }
        }

        block->free(mem);
}

Ref<Alloc::MemBlock>
Alloc::MemBlock::copy()
{
        auto res = NewRef<MemBlock>();
        res->member_size = member_size;
        res->size = size;
        register_range(res);
        return res;
}

void
Alloc::MemBlock::register_range(Ref<MemBlock> block)
{
        static uint16_t index = 0;
        block->index = index++;
        s_MemBlocksBT.emplace_back(RCAST<uint64_t>(block->begin()), block);
        std::sort(s_MemBlocksBT.begin(), s_MemBlocksBT.end(),
                  [](auto a, auto b) { return a.first < b.first; });
}

std::vector<std::pair<uint64_t, Ref<Alloc::MemBlock>>> Alloc::s_MemBlocksBT;

}  // namespace Borek
