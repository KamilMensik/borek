// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <array>
#include <string>
#include <format>
#include <cstdint>
#include <vector>
#include <iostream>

#include <mruby.h>
#include <mruby/variable.h>

#include "helper_macros.h"

#define MRB_FUNC(_name) \
mrb_value _name(mrb_state* mrb, mrb_value self)

#define MRB_LAMBDA \
[](mrb_state* mrb, mrb_value self) -> mrb_value

#define MRB_NIL mrb_nil_value()
#define MRB_NUM(_val) mrb_fixnum_value(_val)
#define MRB_FLOAT(_val) mrb_float_value(mrb, _val)
#define MRB_STRING(_val) mrb_str_new_cstr(mrb, _val)
#define MRB_SYM(_val) mrb_intern_cstr(mrb, _val)
#define MRB_BOOL(_val) mrb_bool_value(_val)

#define MRB_OPT_KWARG(_index, _val) \
if (mrb_undef_p(kw_values[_index])) { kw_values[_index] = MRB_NUM(UINT32_MAX); }

#define MRB_OPT()

#define MRB_KWARGS(...) \
        const char *kw_names[PP_NARG(__VA_ARGS__)] = { __VA_ARGS__ }; \
        mrb_value kw_values[PP_NARG(__VA_ARGS__)];                    \
        const mrb_kwargs kwargs = {                                   \
                .num = PP_NARG(__VA_ARGS__),                          \
                .required = 0,                                        \
                .table = (mrb_sym*)kw_names,                          \
                .values = kw_values,                                  \
                .rest = NULL,                                         \
        };

#define MRB_SET_IV(_obj, _name, _val) \
        mrb_iv_set(mrb, _obj, mrb_intern_cstr(mrb, _name), _val)

#define MRB_GET_IV(_obj, _name) \
        mrb_iv_get(mrb, _obj, mrb_intern_cstr(mrb, _name))

#define MRB_IV_DEFINED(_obj, _name) \
        mrb_iv_defined(mrb, _obj, mrb_intern_cstr(mrb, _name))

#define MRB_CONST_GET(_obj, _name) \
        mrb_const_get(mrb, _obj, mrb_intern_cstr(mrb, _name))

#define MRB_CONST_SET(_obj, _name, _val) \
        mrb_const_set(mrb, _obj, mrb_intern_cstr(mrb, _name), _val)

#define MRB_FUNCALL(_obj, _func, ...) \
      mrb_funcall(mrb, _obj, _func, PP_NARG(__VA_ARGS__)__VA_OPT__(, __VA_ARGS__))

#define MRB_ARG1 mrb_get_arg1(mrb)

namespace mrbcpp {

template<unsigned N>
struct FixedString {
        char buf[N + 1]{};
        constexpr FixedString(char const* s) {
                for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
        }
        constexpr operator char const*() const { return buf; }
        constexpr const char* c_str() const { return *this; }

};

template <FixedString str>
static const char* c_str()
{
        return str.c_str();
}

template<unsigned N> FixedString(char const (&)[N]) -> FixedString<N - 1>;

class FuncArgs {
public:
        FuncArgs();
        FuncArgs& Required(unsigned n);
        FuncArgs& Optional(unsigned n);
        operator mrb_aspec();

private:
        mrb_aspec val;
};

typedef mrb_value (*mrbcpp_func_t)(mrb_state *mrb, mrb_value self);

class Class {
public:
        Class() = default;
        Class(mrb_state* vm, RClass* self);
        Class& define_method(const std::string& name, mrbcpp_func_t func, FuncArgs args = FuncArgs());
        Class& define_class_method(const std::string& name, mrbcpp_func_t func, FuncArgs args = FuncArgs());
        Class& define_const(const std::string& name, mrb_value val);
        Class& define_class_iv(const std::string& name, mrb_value val);
        
        template <FixedString name>
        Class& define_attr_reader()
        {
                mrb_define_method(m_VM, m_Self, name, MRB_LAMBDA {
                        return MRB_GET_IV(self, std::format("@{}", (const char*)name).c_str());
                }, MRB_ARGS_NONE());

                return *this;
        }

        template <FixedString name>
        Class& define_attr_writer()
        {
                mrb_define_method(m_VM, m_Self, std::format("{}=", (const char*)name).c_str(), MRB_LAMBDA {
                        MRB_SET_IV(self, std::format("@{}", (const char*)name).c_str(), MRB_ARG1);
                        return MRB_ARG1;
                }, MRB_ARGS_REQ(1));

                return *this;
        }

        template <FixedString name>
        Class& define_attr_accessor()
        {
                define_attr_reader<name>();
                define_attr_writer<name>();

                return *this;
        }

        operator RClass*();
        operator mrb_value();

private:
        mrb_state* m_VM;
        RClass* m_Self;
};

class Module {
public:
        Module() = default;
        Module(mrb_state* vm, RClass* self);
        Class define_class(const std::string& name);
        Class define_class(const std::string& name, Class super);
        Module define_module(const std::string& name);
        Module& define_const(const std::string& name, mrb_value val);
        Class get_class(const std::string& name);

        operator RClass*();
private:
        mrb_state* m_VM;
        RClass* m_Self;
};

class VM {
public:
        VM();
        explicit VM(mrb_state* state);
        Class define_class(const std::string& name);
        Class define_class(const std::string& name, Class super);
        Module define_module(const std::string& name);
        operator mrb_state*();
        mrb_state* GetSelf();
        void Destroy();

        constexpr mrb_value value_from(RClass* c)
        {
                mrb_value res;
                res.w = (uint64_t)c;
                return res;
        }

private:
        mrb_state* m_Self;
};

}  // namespace mrbcpp
