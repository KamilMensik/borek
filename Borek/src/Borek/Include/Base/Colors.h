// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Debug/Log.h"
#include "glm/common.hpp"
#include <cstdint>
#include <format>
#include <glm/ext/vector_float4.hpp>

namespace Borek {

namespace Colors {

//static glm::vec4 FROM_RGB(uint8_t r, uint8_t g, uint8_t b)
//{
//        return glm::vec4(r / 256.0f, g / 256.0f, b / 256.0f, 1.0f);
//}

static constexpr glm::vec4 FROM_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
        return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

}

using ColorF = glm::vec4;

struct Color {
        uint8_t r, g, b, a;

        Color() {}
        Color(const ColorF& color);
        Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

        operator glm::vec4() const;

        bool
        operator ==(const Color& other) const
        {
                return r == other.r && g == other.g && b == other.b && a == other.a;
        }

        operator std::string()
        {
                return std::format("[{}, {}, {}, {}]", r, g, b, a);
        }

        Color
        operator *(float amount) const
        {
                return Color(r * amount, g * amount, b * amount, a * amount);
        }

        Color&
        operator *=(float amount)
        {
                r *= amount;
                g *= amount;
                b *= amount;
                a *= amount;
                return *this;
        }

        Color
        operator +(const Color& other) const
        {
                return Color(r + other.r, g + other.g, b + other.b, a * other.a);
        }

        Color&
        operator +=(const Color& other)
        {
                r += other.r;
                g += other.g;
                b += other.b;
                a += other.a;
                return *this;
        }
};

namespace Colors {

static const Color RED = Color(242, 38, 19);
static const Color GREEN = Color(46, 204, 113);
static const Color BLUE = Color(43, 44, 170);
static const Color YELLOW = Color(255, 191, 0);
static const Color CYAN = Color(0, 238, 222);
static const Color WHITE = Color(255, 255, 255);

}  // namespace Colors

}  // namespace Borek
