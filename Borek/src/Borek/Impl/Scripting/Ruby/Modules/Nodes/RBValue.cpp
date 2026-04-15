// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <unordered_map>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/boxing_word.h>
#include <mruby/hash.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mrbcpp.h>

#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBValue.h"
#include "Include/Base/Entity.h"
#include "Include/Components/ValueComponent.h"
#include "Include/Scripting/Ruby/Modules/Nodes/RBNode.h"
#include "Include/Base/Application.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static std::unordered_map<uint32_t, uint32_t> s_SetterTranslationMap;


static inline mrb_value
ToRbValue(mrb_state* mrb, const Value& val)
{
        switch (val.type) {
        case ValueType_String:
                return mrb_str_new_cstr(mrb, val.str.Str().c_str());
        case ValueType_Bool:
                return mrb_bool_value(val.boolean);
        case ValueType_Number:
                return mrb_float_value(mrb, val.number);
        case ValueType_Int:
                return mrb_int_value(mrb, val.integer);
        default:
                return MRB_NIL;
        }
}

static MRB_FUNC(Get)
{
        mrb_value res = mrb_hash_get(mrb, MRB_GET_IV(self, "_@vals"),
                                     mrb_symbol_value(mrb_get_mid(mrb)));

        if (mrb_nil_p(res)) {
                mrb_raise(mrb, E_NOMETHOD_ERROR, "");
                return MRB_NIL;
        }

        return res;
}

static MRB_FUNC(Set)
{
        auto it = s_SetterTranslationMap.find(mrb_get_mid(mrb));
        if (it == s_SetterTranslationMap.end()) {
                mrb_raise(mrb, E_NOMETHOD_ERROR, "");
                return MRB_NIL;
        }

        mrb_value hash = MRB_GET_IV(self, "_@vals");
        mrb_value key = mrb_symbol_value(it->second);

        if (!mrb_hash_key_p(mrb, hash, key)) {
                mrb_raise(mrb, E_NOMETHOD_ERROR, "");
                return MRB_NIL;
        }

        mrb_hash_set(mrb, hash, key, MRB_ARG1);

        return MRB_NIL;
}

void
RBValue::InitAccessors(mrb_value self)
{
        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        Class& cls = RBNode::node_class;
        Module& mdl = RBNode::node_module;

        Entity e(mrb_fixnum(MRB_GET_IV(self, "@entity_id")));
        auto& vc = e.GetComponent<ValueComponent>();
        mrb_value hash = mrb_hash_new(mrb);
        MRB_SET_IV(self, "_@vals", hash);


        for (auto& value : vc) {
                Symbol getter_sym = value.name;
                Symbol setter_sym (getter_sym.Str() + '=');

                mrb_hash_set(mrb, hash, mrb_symbol_value(value.name.RBSym()),
                             ToRbValue(mrb, value));

                if (s_SetterTranslationMap.contains(setter_sym.RBSym()))
                        continue;

                s_SetterTranslationMap[setter_sym.RBSym()] = getter_sym.RBSym();

                mrb_define_method(mrb, cls, getter_sym.Str().c_str(), Get, FuncArgs());
                mrb_define_method(mrb, mdl, getter_sym.Str().c_str(), Get, FuncArgs());

                mrb_define_method(mrb, cls, setter_sym.Str().c_str(), Set,
                                  FuncArgs().Required(1));
                mrb_define_method(mrb, mdl, setter_sym.Str().c_str(), Set,
                                  FuncArgs().Required(1));
        }
}

}  // namespace RBModules
}  // namespace vm
