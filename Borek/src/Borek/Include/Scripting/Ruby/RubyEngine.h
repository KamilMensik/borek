// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Scripting/Ruby/Compiler.h"
#include <mrbcpp.h>

struct mrb_state;
struct RClass;

namespace Borek {

class RubyEngine {
friend class RBModule;
public:
        RubyEngine();
        ~RubyEngine();
        mrbcpp::VM& GetRubyVM();
        mrbcpp::Module& GetBorekModule();
        Compiler& GetCompiler();
        std::vector<std::string>& GetClasses();
        void Run(const std::string& code);

private:
        mrbcpp::VM m_RubyVM;
        mrbcpp::Module m_BorekModule;
        Compiler m_Compiler;
        std::vector<std::string> m_DefinedClasses;

        void InitModules();
};

}  // namespace Borek
