#pragma once

#include <cstdint>
#include <memory>

#define EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define SCAST static_cast
#define DCAST dynamic_cast
#define RCAST reinterpret_cast
#define VPCAST(_val) RCAST<void*>(SCAST<uint64_t>(_val))

#define BOREK_APPLICATION(_application_class)   \
int main(int argc, char** argv)                 \
{                                               \
        auto app = new _application_class();    \
        app->Run();                             \
        delete app;                             \
                                                \
        return 0;                               \
}

namespace Borek {

template <class T>
using Ref = std::shared_ptr<T>;

template <class T>
using Uniq = std::unique_ptr<T>;

template <class T, class ... Args>
inline Ref<T> NewRef(Args&&... args)
{
        return std::make_shared<T>(std::forward<Args>(args)...);
}

template <class T, class ... Args>
inline Uniq<T> NewUniq(Args&&... args)
{
        return std::make_unique<T>(std::forward<Args>(args)...);
}

// Enum with its members values being its sizes.
enum class Datatype : uint8_t {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        Sampler2D,
};

class Time {
public:
        Time(double time) : m_Time(time) {}
        inline double Seconds() const { return m_Time; }
        inline double Miliseconds() const { return m_Time * 1000; }

private:
        double m_Time;
};

uint32_t DatatypeSize(Datatype d);

struct BitFlags {
        uint32_t val;

        BitFlags() : val(0) {}
        BitFlags(uint32_t val) : val(val) {} 

        inline void
        SetFlags(uint32_t flags, bool value)
        {
                if (value)
                        val |= flags;
                else
                        val &= ~flags;
        }

        inline bool
        HasFlags(uint32_t flags) const { return val & flags; }

        inline void
        operator=(uint32_t value) { val = value; }

        inline operator uint32_t() const { return val; }

        inline uint32_t
        operator&(uint32_t value) const { return val & value; }

        inline uint32_t
        operator|(uint32_t value) const { return val | value; }

        inline uint32_t
        operator~() const { return ~val; }
};

}  // namespace Borek

#ifndef _BOREK_NOT_USE_REFS
        using Borek::Ref, Borek::Uniq;
#endif

#ifdef _ASSETS_PATH
#define ASSET_PATH(_relative) (_ASSETS_PATH _relative)
#else
#define ASSET_PATH(_relative) (_PROJECT_PATH _relative)
#endif
