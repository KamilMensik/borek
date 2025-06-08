// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <string>
#include <vector>

namespace Borek {

struct String {
        std::vector<char> value;
        String() {};
        String(const std::string& val) : value(val.begin(), val.end()) {}

        operator std::string() { return std::string(value.data()); }
        operator const char*() { return value.data(); }
        void operator =(const std::string& str) { value = std::vector<char>(str.begin(), str.end()); value.emplace_back('\0'); }
        const char* c_str() { return value.data(); }

};

}  // namespace Borek
