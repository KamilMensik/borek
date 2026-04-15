// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Colors.h"

namespace Borek {

Color::Color(const ColorF& color)
{
        const glm::vec4 tvec(255, 255, 255, 255);
        glm::vec4 new_vec = glm::ceil(color * tvec);
        r = new_vec.r;
        g = new_vec.g;
        b = new_vec.b;
        a = new_vec.a;
}

Color::operator glm::vec4() const
{
        return Colors::FROM_RGBA(r, g, b, a);
}


}  // namespace Borek
