// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>
#include <mruby/array.h>
#include <mruby/common.h>
#include <mruby/class.h>
#include <mruby/internal.h>
#include <mruby/object.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

#include <mrbcpp.h>
#include <ECS/Component.h>

#include "Include/Core.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBNDatatypes.h"
#include "Include/Scripting/Ruby/Modules/RBDatatypes.h"
#include "Include/Debug/Log.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

/*
 *      Notifying vector class
 */


#define VEC_ARITHMETIC_OP(_name, _type, _op)                                   \
static MRB_FUNC(_name)                                                         \
{                                                                              \
        mrb_value res = mrb_class_new_instance(mrb, 0, nullptr,                \
                                               RBDatatypes:: _type##_class);   \
                                                                               \
        auto data = MRB_ISTRUCT_VAL(res, glm:: _type);                         \
        *data = MRB_ISTRUCT_VAL(self, RBN##_type)->vals;                       \
                                                                               \
        mrb_value arg = MRB_ARG1;                                              \
        if (mrb_integer_p(arg)) {                                              \
                *data _op##= mrb_fixnum(arg);                                  \
        } else if (mrb_float_p(arg)) {                                         \
                *data _op##= mrb_float(arg);                                   \
        } else if (mrb_type(arg) == MRB_TT_ISTRUCT) {                          \
                auto stru = RCAST<RIStruct*>(arg.w);                           \
                if (stru->c != RBDatatypes:: _type##_class &&                  \
                    stru->c != RBNDatatypes:: _type##_class) {                 \
                        BOREK_ENGINE_ERROR("Arithmetic operation error");      \
                        return MRB_NIL;                                        \
                }                                                              \
                auto other_data = RCAST<glm:: _type *>(stru->inline_data);     \
                *data _op##= *other_data;                                      \
        } else {                                                               \
                BOREK_ENGINE_ERROR("Arithmetic operation error");              \
                return MRB_NIL;                                                \
        }                                                                      \
                                                                               \
        return res;                                                            \
}

#define VEC_ARITHMETIC_OPERATIONS(_name, _type)                                \
VEC_ARITHMETIC_OP(_name##Add, _type, +)                                        \
VEC_ARITHMETIC_OP(_name##Sub, _type, -)                                        \
VEC_ARITHMETIC_OP(_name##Mul, _type, *)                                        \
VEC_ARITHMETIC_OP(_name##Div, _type, /)

struct RBNvec2 {
        glm::vec2 vals;
        float padding[2];
        mrb_value delegate;
};

struct RBNvec3 {
        glm::vec3 vals;
        float padding;
        mrb_value delegate;
};

struct RBNvec4 {
        glm::vec4 vals;
        mrb_value delegate;
};

VEC_ARITHMETIC_OPERATIONS(NVec2, vec2)
VEC_ARITHMETIC_OPERATIONS(NVec3, vec3)
VEC_ARITHMETIC_OPERATIONS(NVec4, vec4)

static MRB_FUNC(Notify)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        MRB_FUNCALL(data->delegate, "notify");
        return MRB_NIL;
}

static MRB_FUNC(VecFirst)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        return mrb_float_value(mrb, data->vals.x);
}

static MRB_FUNC(VecFirstSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        data->vals.x = arg;

        Notify(mrb, self);
        return MRB_NIL;
}

static MRB_FUNC(VecSecond)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        return mrb_float_value(mrb, data->vals.y);
}

static MRB_FUNC(VecSecondSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        data->vals.y = arg;

        Notify(mrb, self);
        return MRB_NIL;
}

static MRB_FUNC(VecThird)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        return mrb_float_value(mrb, data->vals.z);
}

static MRB_FUNC(VecThirdSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        data->vals.z = arg;

        Notify(mrb, self);
        return MRB_NIL;
}

static MRB_FUNC(VecFourth)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        return mrb_float_value(mrb, data->vals.w);
}

static MRB_FUNC(VecFourthSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        data->vals.w = arg;

        Notify(mrb, self);
        return MRB_NIL;
}

/*
 *      Vec2 class
 */

static MRB_FUNC(Vec2Initialize)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec2);
        mrb_float x = 0;
        mrb_float y = 0;

        mrb_get_args(mrb, "o|ff", &data->delegate, &x, &y);

        data->vals.x = x;
        data->vals.y = y;

        return self;
}

static MRB_FUNC(Vec2ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, RBNvec2);
        std::string str = std::format("Vec2[@{}, {}, {}]",
                                      mrb_string_cstr(mrb, mrb_obj_as_string(mrb, data->delegate)),
                                      data->vals.x, data->vals.y);
        return mrb_str_new_cstr(mrb, str.c_str());
}

/*
 *      Vec3 class
 */
static MRB_FUNC(Vec3Initialize)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec3);
        mrb_float x = 0;
        mrb_float y = 0;
        mrb_float z = 0;

        mrb_get_args(mrb, "o|fff", &data->delegate, &x, &y, &z);

        data->vals.x = x;
        data->vals.y = y;
        data->vals.z = z;

        return self;
}

static MRB_FUNC(Vec3ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, RBNvec3);
        std::string str = std::format("Vec3[{}, {}, {}]", data->vals.x,
                                      data->vals.y, data->vals.z);
        return mrb_str_new_cstr(mrb, str.c_str());
}

static MRB_FUNC(Vec4Initialize)
{
        auto data = MRB_ISTRUCT_VAL(self, RBNvec4);
        mrb_float x = 0;
        mrb_float y = 0;
        mrb_float z = 0;
        mrb_float w = 0;

        mrb_get_args(mrb, "o|ffff", &data->delegate, &x, &y, &z, &w);

        data->vals.x = x;
        data->vals.y = y;
        data->vals.z = z;
        data->vals.w = w;

        return self;
}

static MRB_FUNC(Vec4ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, RBNvec4);
        std::string str = std::format("Vec3[{}, {}, {}, {}]", data->vals.x,
                                      data->vals.y, data->vals.z, data->vals.w);
        return mrb_str_new_cstr(mrb, str.c_str());
}

void RBNDatatypes::Init(RubyEngine& engine)
{
        VM& vm = engine.GetRubyVM();

        vec2_class = vm.define_class("NVec2", MRB_TT_ISTRUCT)
                .define_method("initialize", Vec2Initialize, FuncArgs().Optional(2))
                .define_method("x", VecFirst)
                .define_method("x=", VecFirstSet, FuncArgs().Required(1))
                .define_method("y", VecSecond)
                .define_method("y=", VecSecondSet, FuncArgs().Required(1))
                .define_method("+", NVec2Add, FuncArgs().Required(1))
                .define_method("-", NVec2Sub, FuncArgs().Required(1))
                .define_method("*", NVec2Mul, FuncArgs().Required(1))
                .define_method("/", NVec2Div, FuncArgs().Required(1))
                .define_method("to_s", Vec2ToS);

        vec3_class = vm.define_class("NVec3", MRB_TT_ISTRUCT)
                .define_method("initialize", Vec3Initialize, FuncArgs().Optional(3))
                .define_method("x", VecFirst)
                .define_method("x=", VecFirstSet, FuncArgs().Required(1))
                .define_method("y", VecSecond)
                .define_method("y=", VecSecondSet, FuncArgs().Required(1))
                .define_method("z", VecThird)
                .define_method("z=", VecThirdSet, FuncArgs().Required(1))
                .define_method("r", VecFirst)
                .define_method("r=", VecFirstSet, FuncArgs().Required(1))
                .define_method("g", VecSecond)
                .define_method("g=", VecSecondSet, FuncArgs().Required(1))
                .define_method("b", VecThird)
                .define_method("b=", VecThirdSet, FuncArgs().Required(1))
                .define_method("+", NVec3Add, FuncArgs().Required(1))
                .define_method("-", NVec3Sub, FuncArgs().Required(1))
                .define_method("*", NVec3Mul, FuncArgs().Required(1))
                .define_method("/", NVec3Div, FuncArgs().Required(1))
                .define_method("to_s", Vec3ToS);

        vec4_class = vm.define_class("NVec4", MRB_TT_ISTRUCT)
                .define_method("initialize", Vec4Initialize, FuncArgs().Optional(4))
                .define_method("x", VecFirst)
                .define_method("x=", VecFirstSet, FuncArgs().Required(1))
                .define_method("y", VecSecond)
                .define_method("y=", VecSecondSet, FuncArgs().Required(1))
                .define_method("z", VecThird)
                .define_method("z=", VecThirdSet, FuncArgs().Required(1))
                .define_method("w", VecFourth)
                .define_method("w=", VecFourthSet, FuncArgs().Required(1))
                .define_method("r", VecFirst)
                .define_method("r=", VecFirstSet, FuncArgs().Required(1))
                .define_method("g", VecSecond)
                .define_method("g=", VecSecondSet, FuncArgs().Required(1))
                .define_method("b", VecThird)
                .define_method("b=", VecThirdSet, FuncArgs().Required(1))
                .define_method("a", VecFourth)
                .define_method("a=", VecFourthSet, FuncArgs().Required(1))
                .define_method("+", NVec4Add, FuncArgs().Required(1))
                .define_method("-", NVec4Sub, FuncArgs().Required(1))
                .define_method("*", NVec4Mul, FuncArgs().Required(1))
                .define_method("/", NVec4Div, FuncArgs().Required(1))
                .define_method("to_s", Vec4ToS);
}

Class RBNDatatypes::vec2_class;
Class RBNDatatypes::vec3_class;
Class RBNDatatypes::vec4_class;

}  // namespace RBModules
}  // namespace Borek
