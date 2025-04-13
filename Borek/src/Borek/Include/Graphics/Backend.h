// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <string>

namespace Borek {

namespace Graphics {

class Backend {
public:
        enum class Type {
                kOpenGL
        };

        static inline bool CheckType(Type type) { return s_Type == type; }
        static inline Type GetType() { return s_Type; }
        static bool SetType(const std::string& name);
        static bool SetType(Type type);

private:
        static Type s_Type;
};

} // namespace Graphics

}  // namespace Borek
