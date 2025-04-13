// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glad/glad.h>

#include "Include/Graphics/BufferDescriptor.h"

namespace Borek {
namespace Graphics {

using BDElementVector = std::vector<BufferDescriptor::Element>;
using ElementIList = std::initializer_list<BufferDescriptor::Element>;

uint32_t BufferDescriptor::Element::Count() const
{
        switch (datatype) {
        case Datatype::Float:
                return 1;
        case Datatype::Float2:
                return 2;
        case Datatype::Float3:
                return 3;
        case Datatype::Float4:
                return 4;
        }

        return 0;
}

// Function that calculates offsets for every element of BufferDescriptor class.
// Returns stride of the BufferDescriptor.
static inline uint32_t CalculateOffset(BDElementVector& els)
{
        uint32_t offset = 0;

        for (auto& element : els) {
                element.offset = offset;
                offset += element.size;
        }

        return offset;
}

BufferDescriptor::Element::Element(Datatype datatype, const std::string& name,
                                   bool is_normalized)
{
        this->name = name;
        this->size = DatatypeSize(datatype);
        this->datatype = datatype;
        this->is_normalized = is_normalized;
}

BufferDescriptor::BufferDescriptor(const ElementIList& elements)
        : m_Elements(elements)
{
        m_Stride = CalculateOffset(m_Elements);
}

const uint32_t BufferDescriptor::Size()
{
        uint32_t res = 0;

        for (const auto& el : m_Elements) {
                res += el.size;
        }

        return res;
}


}  // namespace Graphics
}  // namespace Borek
