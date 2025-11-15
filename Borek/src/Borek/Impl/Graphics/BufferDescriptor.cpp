// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
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
        case Datatype::Int:
                return 1;
        case Datatype::Float2:
        case Datatype::Int2:
        case Datatype::Sampler2D:
                return 2;
        case Datatype::Float3:
        case Datatype::Int3:
                return 3;
        case Datatype::Float4:
        case Datatype::Int4:
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

uint32_t BufferDescriptor::Size()
{
        return m_Stride;
}


}  // namespace Graphics
}  // namespace Borek
