// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <vector>
#include <string>

#include "Include/Core.h"

namespace Borek {
namespace Graphics {

class BufferDescriptor {
public:
        struct Element {
                std::string name;
                uint32_t offset;
                uint32_t size;
                Datatype datatype;               
                bool is_normalized;

                Element(Datatype datatype, const std::string& name,
                        bool is_normalized = false);
                uint32_t Count() const;
        };

        BufferDescriptor(const std::initializer_list<Element>& elements);
        BufferDescriptor() {}

        uint32_t Size();

        inline const std::vector<Element>& GetElements()
        {
                return m_Elements;
        }

        inline std::vector<Element>::iterator begin()
        {
                return m_Elements.begin();
        }

        inline std::vector<Element>::iterator end()
        {
                return m_Elements.end();
        }

private:
        std::vector<Element> m_Elements;
        uint32_t m_Stride;
};

}  // namespace Graphics
}  // namespace Borek
