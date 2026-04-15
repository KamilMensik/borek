// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Include/Core.h"

namespace Borek {

class Symbol {
public:
        Symbol();

        Symbol(std::string_view value);

        Symbol(const std::string& value);

        Symbol(const char* value);

        Symbol(uint32_t value);

        ~Symbol();

        Symbol(const Symbol& other);

        Symbol(Symbol&& other);

        const std::string&
        Str() const;

        uint32_t
        Value() const;

        uint32_t
        RBSym() const;

        void
        operator =(const Symbol& other);

        void
        operator =(Symbol&& other);

        bool
        operator ==(const Symbol& other) const;

        operator const std::string&() const;
        operator uint32_t() const;

        bool
        operator <(const Symbol& other) const;

        inline operator bool() const { return !IsNil(); }
        inline bool IsNil() const { return m_Value == UINT32_MAX; }

        static Symbol
        FromRB(uint32_t sym);

private:
        uint32_t m_Value;

        static StringMap<uint32_t> s_StringMap;
        static Map<uint32_t, uint32_t> s_RBSymMap;
        static std::vector<std::string> s_StringVec;
        static std::vector<uint32_t> s_RBSymVec;
        static std::vector<uint32_t> s_StringReferences;
        static uint32_t s_FirstFreeSpot;

        void
        Increment();

        void
        Decrement();

        static uint32_t
        GetID();
};

}  // namespace Borek

template<> struct std::hash<Borek::Symbol> {
    std::size_t operator()(Borek::Symbol const& s) const noexcept {
        return std::hash<uint32_t>{}(s);
    }
};
