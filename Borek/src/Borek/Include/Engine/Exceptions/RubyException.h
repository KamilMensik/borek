// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <exception>
#include <string>
#include <string_view>

namespace Borek {

class RubyException : public std::exception {
public:

        RubyException(std::string_view msg) : m_Msg(msg) {}

        const char* what() const noexcept {
                return m_Msg.c_str();
        }

private:
        std::string m_Msg;
};

}  // namespace Borek
