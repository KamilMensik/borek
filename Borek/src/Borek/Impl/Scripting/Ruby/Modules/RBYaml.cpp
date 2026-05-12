// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "yaml-cpp/exceptions.h"
#include <cmath>
#include <sstream>

#include <mrbcpp.h>
#include <mruby/istruct.h>
#include <mruby/data.h>
#include <mruby/proc.h>
#include <mruby/string.h>
#include <mruby/boxing_word.h>
#include <mruby/class.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/value.h>
#include <mruby/numeric.h>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/emitter.h>

#include "Include/Base/File.h"
#include "Include/Engine/Alloc.h"
#include "Include/Scripting/Ruby/Modules/RBYaml.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include "Include/Scripting/Ruby/Modules/RBFile.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

static YAML::Node
to_yaml_helper(mrb_state* mrb, mrb_value self);

static mrb_value
flo_to_s(mrb_state *mrb, mrb_value flt)
{
        mrb_float f = mrb_float(flt);
        mrb_value str;

        if (isinf(f)) {
        str = f < 0 ? mrb_str_new_lit(mrb, "-inf") : mrb_str_new_lit(mrb, "inf");
        }
        else if (isnan(f)) {
                str = mrb_str_new_lit(mrb, "NaN");
        }
        else {
                str = mrb_float_to_str(mrb, flt, NULL);
        }

        RSTR_SET_ASCII_FLAG(mrb_str_ptr(str));
        return str;
}

static void
yaml_free(mrb_state* mrb, void* node)
{
        YAML::Node* cast = RCAST<YAML::Node*>(node);

        cast->~Node();
        Alloc::Free(RCAST<uint64_t*>(cast));
}

static const mrb_data_type
yaml_data_type {
        .struct_name = "YAML",
        .dfree = yaml_free,
};

static MRB_FUNC(Initialize)
{
        mrb_value arg = MRB_ARG1;
        
        YAML::Node* node;

        try {
                if (mrb_string_p(arg)) {
                        node = RCAST<YAML::Node*>(Alloc::Allocate<8>());
                        new(node) YAML::Node(YAML::Load(mrb_string_cstr(mrb, arg)));
                } else if (mrb_data_p(arg) && mrb_class(mrb, arg) == RBFile::file_class) {
                        node = RCAST<YAML::Node*>(Alloc::Allocate<8>());
                        File* f = RCAST<File*>(DATA_PTR(arg));
                        new(node) YAML::Node(YAML::Load(f->Get()));
                } else {
                        mrb_raise(mrb, E_ARGUMENT_ERROR, "argument must be either yaml string or file");
                        return MRB_NIL;
                }
        } catch (YAML::Exception e) {
                mrb_raise(mrb, E_RUNTIME_ERROR, e.msg.c_str());
                return MRB_NIL;
        }

        mrb_data_init(self, node, &yaml_data_type);
        return self;
}

static MRB_FUNC(YamlParse)
{
        mrb_value arg = MRB_ARG1;
        return mrb_class_new_instance(mrb, 1, &arg, RBYaml::yaml_class);
}

static MRB_FUNC(ToS)
{
        std::stringstream res;
        auto node = RCAST<YAML::Node*>(mrb_data_get_ptr(mrb, self, &yaml_data_type));

        node->SetStyle(YAML::EmitterStyle::Block);
        res << *node;

        std::string str = res.str();
        return mrb_str_new(mrb, str.c_str(), str.length());
}

static mrb_value
try_parse_num(mrb_state* mrb, const std::string& str)
{
        enum state { INTEGRAL, DECIMAL, EXPONENTIAL };

        state s = INTEGRAL;
        bool failed = false;
        bool negative = false;
        bool exponent_negative = false;
        bool needs_number = true;
        mrb_int accumulator = 0;
        int decimal_divider = 1;
        double dres;

        for (char c : str) {
                if (failed) return mrb_str_new(mrb, str.c_str(), str.length());

                switch (c) {
                case '+':
                        switch (s) {
                        case DECIMAL:
                                failed = true;
                                break;
                        case INTEGRAL: case EXPONENTIAL:
                                if (!needs_number)
                                        failed = true;

                                break;
                        }
                        break;
                case '-':
                        switch (s) {
                        case DECIMAL:
                                failed = true;
                                break;
                        case INTEGRAL:
                                if (!needs_number)
                                        failed = true;

                                negative = true;
                                break;
                        case EXPONENTIAL:
                                if (!needs_number)
                                        failed = true;

                                exponent_negative = true;
                                break;
                        }
                        break;
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        if (s == DECIMAL)
                                decimal_divider *= 10;

                        accumulator = accumulator * 10 + c - '0';
                        needs_number = false;
                        break;
                case '.':
                        if (s != INTEGRAL) {
                                failed = true;
                                break;
                        }

                        needs_number = true;
                        dres = accumulator;
                        s = DECIMAL;
                        accumulator = 0;
                        break;
                case 'e': case 'E':
                        switch (s) {
                        case EXPONENTIAL:
                                failed = true;
                                break;
                        case DECIMAL:
                                if (needs_number) {
                                        failed = true;
                                        break;
                                }

                                dres += double(accumulator) / decimal_divider;
                                accumulator = 0;
                                needs_number = true;
                                s = EXPONENTIAL;
                                break;
                        case INTEGRAL:
                                dres = accumulator;
                                needs_number = true;
                                accumulator = 0;
                                s = EXPONENTIAL;
                                break;
                        }
                        break;
                default:
                        failed = true;
                        break;
                }
        }

        switch (s) {
        case INTEGRAL:
                if (needs_number)
                        return mrb_str_new(mrb, str.c_str(), str.length());

                return MRB_NUM(negative ? -accumulator : accumulator);
        case DECIMAL:
                if (needs_number)
                        return mrb_str_new(mrb, str.c_str(), str.length());

                dres += double(accumulator) / decimal_divider;
                return MRB_FLOAT(negative ? -dres : dres);
        case EXPONENTIAL:
                if (needs_number)
                        return mrb_str_new(mrb, str.c_str(), str.length());
                
                dres *= pow(10, exponent_negative ? -accumulator : accumulator);
                return MRB_FLOAT(negative ? -dres : dres);
        }
}

static mrb_value
to_rb_helper(mrb_state* mrb, const YAML::Node& node)
{
        mrb_value res;

        switch (node.Type()) {
        case YAML::NodeType::Sequence:
                res = mrb_ary_new_capa(mrb, node.size());
                for (auto child : node) {
                        mrb_ary_push(mrb, res, to_rb_helper(mrb, child));
                }
                break;
        case YAML::NodeType::Map:
                res = mrb_hash_new_capa(mrb, node.size());
                for (YAML::const_iterator it = node.begin(); it != node.end(); it++) {
                        mrb_hash_set(mrb, res, to_rb_helper(mrb, it->first),
                                     to_rb_helper(mrb, it->second));
                }
                break;
        case YAML::NodeType::Undefined:
                res = MRB_NIL;
                break;
        case YAML::NodeType::Null:
                res = MRB_NIL;
                break;
        case YAML::NodeType::Scalar:
                const std::string& str = node.Scalar();
                switch (str[0]) {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                case '+': case '-':
                        res = try_parse_num(mrb, str);
                        break;
                case 't':
                        res = str == "true" ? mrb_true_value() : mrb_str_new(mrb, str.c_str(), str.length());
                        break;
                case 'f':
                        res = str == "false" ? mrb_false_value() : mrb_str_new(mrb, str.c_str(), str.length());
                        break;
                case 'n':
                        res = str == "null" ? mrb_nil_value() : mrb_str_new(mrb, str.c_str(), str.length());
                        break;
                default:
                        res = mrb_str_new(mrb, str.c_str(), str.length());
                }
                break;
        }

        return res;
}

static MRB_FUNC(ToRB)
{
        auto node = RCAST<YAML::Node*>(mrb_data_get_ptr(mrb, self, &yaml_data_type));
        return to_rb_helper(mrb, *node);       
}

static int
to_yaml_helper_hash(mrb_state *mrb, mrb_value key, mrb_value val, void *data)
{
        YAML::Node& res = *RCAST<YAML::Node*>(data);

        if (mrb_symbol_p(key)) {
                res[mrb_sym_name(mrb, mrb_symbol(key))] = to_yaml_helper(mrb, val);
        } else {
                res[mrb_string_cstr(mrb, key)] = to_yaml_helper(mrb, val);
        }

        return 0;
}

static YAML::Node
to_yaml_helper(mrb_state* mrb, mrb_value self)
{
        YAML::Node res;
        switch (mrb_type(self)) {
        case MRB_TT_INTEGER:
                res = mrb_string_cstr(mrb, mrb_integer_to_str(mrb, self, 10));
                break;
        case MRB_TT_STRING:
                res = mrb_string_cstr(mrb, self);
                break;
        case MRB_TT_FLOAT:
                res = mrb_string_cstr(mrb, flo_to_s(mrb, self));
                break;
        case MRB_TT_TRUE:
                res = "true";
                break;
        case MRB_TT_FALSE:
                if (mrb_nil_p(self))
                        res = "null";
                else
                        res = "false";
                break;
        case MRB_TT_ARRAY:
                for (int i = 0; i < RARRAY_LEN(self); i++) {
                        res.push_back(to_yaml_helper(mrb, RARRAY_PTR(self)[i]));
                }
                break;
        case MRB_TT_HASH:
                mrb_hash_foreach(mrb, RHASH(self), to_yaml_helper_hash, &res);
                break;
        default:
                if (mrb_respond_to(mrb, self, mrb_intern_cstr(mrb, "to_h"))) {
                        mrb_value h = MRB_FUNCALL(self, "to_h");
                        mrb_hash_foreach(mrb, RHASH(h), to_yaml_helper_hash, &res);
                } else {
                        res = mrb_string_cstr(mrb, mrb_obj_as_string(mrb, self));
                }
                break;
        }

        return res;
}

static MRB_FUNC(ToYaml)
{
        YAML::Node* node = RCAST<YAML::Node*>(Alloc::Allocate<8>());
        new(node) YAML::Node(to_yaml_helper(mrb, self));

        return mrb_obj_value(mrb_data_object_alloc(mrb, RBYaml::yaml_class, node, &yaml_data_type));
}

static MRB_FUNC(ToJson)
{
        YAML::Node* node = RCAST<YAML::Node*>(Alloc::Allocate<8>());
        new(node) YAML::Node(to_yaml_helper(mrb, self));

        return mrb_obj_value(mrb_data_object_alloc(mrb, RBYaml::json_class, node, &yaml_data_type));
}

static MRB_FUNC(JsonParse)
{
        mrb_value arg = MRB_ARG1;
        return mrb_class_new_instance(mrb, 1, &arg, RBYaml::json_class);
}

static MRB_FUNC(ToSJson)
{
        std::stringstream res;
        auto node = RCAST<YAML::Node*>(mrb_data_get_ptr(mrb, self, &yaml_data_type));

        node->SetStyle(YAML::EmitterStyle::Flow);
        res << *node;

        std::string str = res.str();
        return mrb_str_new(mrb, str.c_str(), str.length());
}

void
RBYaml::Init(class RubyEngine& engine)
{
        auto& vm = engine.GetRubyVM();
        mrb_state* mrb = vm;

        yaml_class = vm.define_class("YAML")
                .define_method("initialize", Initialize, FuncArgs().Required(1))
                .define_class_method("parse", YamlParse, FuncArgs().Required(1))
                .define_method("to_s", ToS)
                .define_method("to_rb", ToRB);

        json_class = vm.define_class("JSON", yaml_class)
                .define_class_method("parse", JsonParse, FuncArgs().Required(1))
                .define_method("to_s", ToSJson);

        mrb_define_method(mrb, mrb->integer_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->float_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->hash_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->array_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->string_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->nil_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->false_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->true_class, "to_yaml", ToYaml, FuncArgs());
        mrb_define_method(mrb, mrb->integer_class, "to_json", ToJson, FuncArgs());
        mrb_define_method(mrb, mrb->float_class, "to_json", ToJson, FuncArgs());
        mrb_define_method(mrb, mrb->hash_class, "to_json", ToJson, FuncArgs());
        mrb_define_method(mrb, mrb->array_class, "to_json", ToJson, FuncArgs());
        mrb_define_method(mrb, mrb->string_class, "to_json", ToJson, FuncArgs());
        mrb_define_method(mrb, mrb->nil_class, "to_json", ToJson, FuncArgs());
        mrb_define_method(mrb, mrb->false_class, "to_json", ToJson, FuncArgs());
        mrb_define_method(mrb, mrb->true_class, "to_json", ToJson, FuncArgs());
}

Class RBYaml::yaml_class;
Class RBYaml::json_class;

}  // namespace RBModules
}  // namespace Borek
