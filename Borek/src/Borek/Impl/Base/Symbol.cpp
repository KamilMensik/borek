// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <cstdint>

#include <iostream>
#include <mrbcpp.h>

#include "Include/Base/Application.h"
#include "Include/Base/Symbol.h"

namespace Borek {

Symbol::Symbol() : m_Value(UINT32_MAX) {}

Symbol::Symbol(std::string_view value)
{
        if (value == "") {
                m_Value = UINT32_MAX;
                return;
        }

        auto it = s_StringMap.find(value);
        if (it != s_StringMap.end()) {
                m_Value = it->second;
        } else {
                m_Value = GetID();
                std::string cpy(value);
                uint32_t rbsym = mrb_intern_cstr(
                        Application::GetRubyEngine().GetRubyVM(), cpy.data());

                s_StringMap[std::move(cpy)] = m_Value;
                s_StringVec[m_Value] = value;
                s_RBSymMap[rbsym] = m_Value;
                s_RBSymVec[m_Value] = rbsym;
        }

        Increment();
}

Symbol::Symbol(const std::string& value) : Symbol(std::string_view(value)) {}

Symbol::Symbol(const char* value) : Symbol(std::string_view(value)) {}

Symbol::Symbol(uint32_t value) : m_Value(value)
{
        if (m_Value != UINT32_MAX)
                Increment();
}

Symbol::~Symbol()
{
        if (m_Value == UINT32_MAX)
                return;

        Decrement();
}

Symbol::Symbol(const Symbol& other)
{
        m_Value = other.m_Value;
        if (m_Value == UINT32_MAX)
                return;

        Increment();
}

Symbol::Symbol(Symbol&& other)
{
        m_Value = other.m_Value;
        other.m_Value = UINT32_MAX;
}

const std::string&
Symbol::Str() const
{
        return *this;
}

uint32_t
Symbol::Value() const
{
        return m_Value;
}

uint32_t
Symbol::RBSym() const
{
        return s_RBSymVec[m_Value];
}

void
Symbol::operator =(const Symbol& other)
{
        if (this == &other)
                return;

        if (m_Value != UINT32_MAX)
                Decrement();

        m_Value = other.m_Value;
        if (m_Value == UINT32_MAX)
                return;

        Increment();
}

void
Symbol::operator =(Symbol&& other)
{
        if (this == &other)
                return;

        if (m_Value != UINT32_MAX)
                Decrement();

        m_Value = other.m_Value;
        other.m_Value = UINT32_MAX;
}

bool
Symbol::operator ==(const Symbol& other) const
{
        return m_Value == other.m_Value;
}

Symbol::operator const std::string&() const
{
        static const std::string empty = "";
        if (m_Value == UINT32_MAX)
                return empty;

        return s_StringVec[m_Value];
}

Symbol::operator uint32_t() const
{
        return m_Value;
}

bool
Symbol::operator <(const Symbol& other) const
{
        return m_Value < other.m_Value;
}

void
Symbol::Increment()
{
        s_StringReferences[m_Value]++;
}

void
Symbol::Decrement()
{
        if (--s_StringReferences[m_Value] != 0)
                return;

        s_StringReferences[m_Value] = s_FirstFreeSpot;
        s_FirstFreeSpot = m_Value;
        s_StringMap.erase(s_StringVec[m_Value]);
        s_RBSymMap.erase(s_RBSymVec[m_Value]);
}

uint32_t Symbol::GetID()
{
        if (s_FirstFreeSpot != UINT32_MAX) {
                const uint32_t res = s_FirstFreeSpot;
                s_FirstFreeSpot = s_StringReferences[s_FirstFreeSpot];
                s_StringReferences[res] = 0;
                return res;
        }

        uint32_t res = s_StringVec.size();
        s_StringVec.emplace_back("");
        s_RBSymVec.emplace_back(UINT32_MAX);
        s_StringReferences.emplace_back(0);
        return res;
}

Symbol Symbol::FromRB(uint32_t sym)
{
        auto it = s_RBSymMap.find(sym);
        if (it == s_RBSymMap.end())
                return Symbol();

        return Symbol(it->second);
}

StringMap<uint32_t> Symbol::s_StringMap;
Map<uint32_t, uint32_t> Symbol::s_RBSymMap;
std::vector<std::string> Symbol::s_StringVec;
std::vector<uint32_t> Symbol::s_RBSymVec;
std::vector<uint32_t> Symbol::s_StringReferences;
uint32_t Symbol::s_FirstFreeSpot = UINT32_MAX;

}  // namespace Borek
