// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>

struct mrb_ccontext;

namespace mrbcpp {
        class VM;
}

namespace Borek {

class Compiler {
public:
        Compiler(mrbcpp::VM& vm);
        ~Compiler();
        void Compile(const std::filesystem::path& in,
                     const std::filesystem::path& out);

private:
        mrb_ccontext* m_Context;
        mrbcpp::VM* m_VM;


};

}  // namespace Borek
