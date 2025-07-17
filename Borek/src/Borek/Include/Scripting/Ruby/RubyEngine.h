// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

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
        std::vector<const char*>& GetClasses();

private:
        mrbcpp::VM m_RubyVM;
        mrbcpp::Module m_BorekModule;
        std::vector<const char*> m_DefinedClasses;
        void InitModules();
};

}  // namespace Borek
