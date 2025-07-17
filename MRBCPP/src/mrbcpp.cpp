// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <ostream>
#include <print>
#include <string>

#include <mruby.h>
#include <mruby/string.h>
#include <mruby/class.h>

#include "mrbcpp.h"

namespace mrbcpp {

FuncArgs::FuncArgs() { val = MRB_ARGS_NONE(); }
FuncArgs& FuncArgs::Required(unsigned n)
{
        val |= MRB_ARGS_REQ(n);
        return *this;
}

FuncArgs& FuncArgs::Optional(unsigned n)
{
        val |= MRB_ARGS_OPT(n);
        return *this;
}

FuncArgs::operator mrb_aspec()
{
        return val;
}

Class::Class(mrb_state* vm, RClass* self) : m_VM(vm), m_Self(self) {}
Class::operator RClass*() { return m_Self; }
Class::operator mrb_value() { return mrb_value{ (uintptr_t)m_Self }; }


Class& Class::define_method(const std::string& name, mrbcpp_func_t func, FuncArgs args)
{
        mrb_define_method(m_VM, m_Self, name.c_str(), func, args);
        return *this;
}

Class& Class::define_class_method(const std::string& name, mrbcpp_func_t func, FuncArgs args)
{
        mrb_define_class_method(m_VM, m_Self, name.c_str(), func, args);
        return *this;
}
Class& Class::define_const(const std::string& name, mrb_value val)
{
        mrb_define_const(m_VM, m_Self, name.c_str(), val);
        return *this;
}

Module::Module(mrb_state* vm, RClass* self) : m_VM(vm), m_Self(self) {}
Module::operator RClass*() { return m_Self; }

Class Module::define_class(const std::string& name)
{
        auto c = mrb_define_class_under(m_VM, m_Self, name.c_str(),
                                        m_VM->object_class);
        MRB_SET_INSTANCE_TT(c, MRB_TT_DATA);
        return Class(m_VM, c);
}

Class Module::define_class(const std::string& name, Class super)
{
        auto c = mrb_define_class_under(m_VM, m_Self, name.c_str(),
                                        super);
        MRB_SET_INSTANCE_TT(c, MRB_TT_DATA);
        return Class(m_VM, c);
}

Module Module::define_module(const std::string& name)
{
        return Module(m_VM,
                      mrb_define_module_under(m_VM, m_Self, name.c_str()));
}

Class Module::get_class(const std::string& name)
{
        return Class(m_VM, mrb_class_get_under(m_VM, m_Self, name.c_str()));
}

Module& Module::define_const(const std::string& name, mrb_value val)
{
        mrb_define_const(m_VM, m_Self, name.c_str(), val);
        return *this;
}


VM::VM()
{
        m_Self = mrb_open();
}

VM::VM(mrb_state* state)
{
        m_Self = state;
}

Class VM::define_class(const std::string& name)
{
        auto c = mrb_define_class(m_Self, name.c_str(), m_Self->object_class);
        MRB_SET_INSTANCE_TT(c, MRB_TT_DATA);

        return Class(m_Self, c);
}

Class VM::define_class(const std::string& name, Class super)
{
        auto c = mrb_define_class(m_Self, name.c_str(), super);
        MRB_SET_INSTANCE_TT(c, MRB_TT_DATA);

        return Class(m_Self, c);
}

Module VM::define_module(const std::string& name)
{
        auto m = mrb_define_module(m_Self, name.c_str());

        return Module(m_Self, m);
}

VM::operator mrb_state*()
{
        return m_Self;
}

mrb_state* VM::GetSelf()
{
        return m_Self;
}

void VM::Destroy()
{
        mrb_close(m_Self);
}

}  // namespace mrbcpp
