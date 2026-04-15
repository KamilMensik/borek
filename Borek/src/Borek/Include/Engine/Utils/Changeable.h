// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

namespace Borek {

template <class T>
class Changeable {
public:
        Changeable(T& item) : m_Item(item) {}
        ~Changeable() { m_Item.OnChange(); }

        T*
        operator ->() { return &m_Item; }

private:
        T& m_Item;
};

}  // namespace Borek
