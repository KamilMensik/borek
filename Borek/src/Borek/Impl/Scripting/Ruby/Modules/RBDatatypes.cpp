// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <format>

#include <mruby.h>
#include <mruby/value.h>
#include <mruby/string.h>
#include <mruby/variable.h>
#include <mruby/value.h>
#include <mruby/data.h>
#include <mruby/istruct.h>
#include <mruby/array.h>
#include <mruby/common.h>
#include <mruby/class.h>
#include <mruby/internal.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_int4.hpp>

#include <mrbcpp.h>
#include <ECS/Component.h>

#include "Include/Core.h"
#include "Include/Scripting/Ruby/RubyEngine.h"
#include "Include/Scripting/Ruby/Modules/RBDatatypes.h"
#include "Include/Scripting/Ruby/Modules/RBNDatatypes.h"
#include "Include/Debug/Log.h"

namespace Borek {
namespace RBModules {

using namespace mrbcpp;

#define VEC_ARITHMETIC_OP(_name, _type, _op)                                   \
static MRB_FUNC(_name)                                                         \
{                                                                              \
        mrb_value res = mrb_class_new_instance(mrb, 0, nullptr,                \
                                               RBDatatypes:: _type##_class);   \
                                                                               \
        auto data = MRB_ISTRUCT_VAL(res, glm:: _type);                         \
        *data = *MRB_ISTRUCT_VAL(self, glm:: _type);                           \
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


/*
 *      Base vector class
 */

static mrb_value
vec_from_items_helper(mrb_state* mrb, mrb_value* values, mrb_int len)
{
        switch (len) {
        case 2:
                return mrb_class_new_instance(mrb, 2, values, RBDatatypes::vec2_class);
        case 3:
                return mrb_class_new_instance(mrb, 3, values, RBDatatypes::vec3_class);
        case 4:
                return mrb_class_new_instance(mrb, 4, values, RBDatatypes::vec4_class);
        default:
                BOREK_LOG_ERROR("Error when trying to create a Vector, unsupported parameter count: {}", len);
                return MRB_NIL;
        }
}

static MRB_FUNC(VecFromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        return vec_from_items_helper(mrb, values, len);
}

static MRB_FUNC(VecFirst)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::vec2);
        return mrb_float_value(mrb, data->x);
}

static MRB_FUNC(VecFirstSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::vec2);
        data->x = arg;

        return MRB_NIL;
}

static MRB_FUNC(VecSecond)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::vec2);
        return mrb_float_value(mrb, data->y);
}

static MRB_FUNC(VecSecondSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::vec2);
        data->y = arg;

        return MRB_NIL;
}

static MRB_FUNC(VecThird)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::vec3);
        return mrb_float_value(mrb, data->z);
}

static MRB_FUNC(VecThirdSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::vec3);
        data->z = arg;
        return MRB_NIL;
}

static MRB_FUNC(VecFourth)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::vec4);
        return mrb_float_value(mrb, data->w);
}

static MRB_FUNC(VecFourthSet)
{
        mrb_float arg;
        mrb_get_args(mrb, "f", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::vec4);
        data->w = arg;
        return MRB_NIL;
}

static MRB_FUNC(Vec2FromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        if (len != 2)
                BOREK_ENGINE_ERROR("Creating Vec2 from incompatible size: {}", len);

        mrb_value instance = mrb_class_new_instance(
                mrb, len, values, RBDatatypes::vec2_class);

        return instance;
}

static MRB_FUNC(Vec2Initialize)
{
        mrb_float x = 0, y = 0;
        mrb_get_args(mrb, "|ff", &x, &y);

        auto* data = MRB_ISTRUCT_VAL(self, glm::vec2);
        data->x = x;
        data->y = y;

        return self;
}

static MRB_FUNC(Vec2ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, glm::vec2);
        std::string str = std::format("Vec2[{}, {}]", data->x, data->y);
        return mrb_str_new_cstr(mrb, str.c_str());
}

static MRB_FUNC(Vec3FromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        if (len != 3)
                BOREK_ENGINE_ERROR("Creating Vec3 from incompatible size: {}", len);

        mrb_value instance = mrb_class_new_instance(
                mrb, len, values, RBDatatypes::vec3_class);

        return instance;
}

static MRB_FUNC(Vec3Initialize)
{
        mrb_float x = 0, y = 0, z = 0;
        mrb_get_args(mrb, "|fff", &x, &y, &z);

        auto* data = MRB_ISTRUCT_VAL(self, glm::vec3);
        data->x = x;
        data->y = y;
        data->z = y;

        return self;
}

static MRB_FUNC(Vec3ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, glm::vec3);
        std::string str = std::format("Vec3[{}, {}, {}]", data->x, data->y,
                                      data->z);
        return mrb_str_new_cstr(mrb, str.c_str());
}

static MRB_FUNC(Vec4FromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        if (len != 4)
                BOREK_ENGINE_ERROR("Creating Vec4 from incompatible size: {}", len);

        mrb_value instance = mrb_class_new_instance(
                mrb, len, values, RBDatatypes::vec4_class);

        return instance;
}

static MRB_FUNC(Vec4Initialize)
{
        mrb_float x = 0, y = 0, z = 0, w = 0;
        mrb_get_args(mrb, "|ffff", &x, &y, &z, &w);

        auto* data = MRB_ISTRUCT_VAL(self, glm::vec4);
        data->x = x;
        data->y = y;
        data->z = y;
        data->w = w;

        return self;
}

static MRB_FUNC(Vec4ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, glm::vec4);
        std::string str = std::format("Vec4[{}, {}, {}, {}]", data->x, data->y,
                                      data->z, data->w);
        return mrb_str_new_cstr(mrb, str.c_str());
}

VEC_ARITHMETIC_OPERATIONS(Vec2, vec2)
VEC_ARITHMETIC_OPERATIONS(Vec3, vec3)
VEC_ARITHMETIC_OPERATIONS(Vec4, vec4)

#define IVEC_ARITHMETIC_OP(_name, _type, _op)                                  \
static MRB_FUNC(_name)                                                         \
{                                                                              \
        mrb_value res = mrb_class_new_instance(mrb, 0, nullptr,                \
                                               RBDatatypes:: _type##_class);   \
                                                                               \
        auto data = MRB_ISTRUCT_VAL(res, glm:: _type);                         \
        *data = *MRB_ISTRUCT_VAL(self, glm:: _type);                           \
                                                                               \
        mrb_value arg = MRB_ARG1;                                              \
        if (mrb_integer_p(arg)) {                                              \
                *data _op##= mrb_fixnum(arg);                                  \
        } else if (mrb_float_p(arg)) {                                         \
                *data _op##= mrb_float(arg);                                   \
        } else if (mrb_type(arg) == MRB_TT_ISTRUCT) {                          \
                auto stru = RCAST<RIStruct*>(arg.w);                           \
                if (stru->c != RBDatatypes:: _type##_class) {                  \
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

#define IVEC_ARITHMETIC_OPERATIONS(_name, _type)                                \
IVEC_ARITHMETIC_OP(_name##Add, _type, +)                                        \
IVEC_ARITHMETIC_OP(_name##Sub, _type, -)                                        \
IVEC_ARITHMETIC_OP(_name##Mul, _type, *)                                        \
IVEC_ARITHMETIC_OP(_name##Div, _type, /)


/*
 *      Base vector class
 */

static mrb_value
ivec_from_items_helper(mrb_state* mrb, mrb_value* values, mrb_int len)
{
        switch (len) {
        case 2:
                return mrb_class_new_instance(mrb, 2, values, RBDatatypes::ivec2_class);
        case 3:
                return mrb_class_new_instance(mrb, 3, values, RBDatatypes::ivec3_class);
        case 4:
                return mrb_class_new_instance(mrb, 4, values, RBDatatypes::ivec4_class);
        default:
                BOREK_LOG_ERROR("Error when trying to create a Vector, unsupported parameter count: {}", len);
                return MRB_NIL;
        }
}

static MRB_FUNC(IVecFromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        return ivec_from_items_helper(mrb, values, len);
}

static MRB_FUNC(IVecFirst)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::ivec2);
        return mrb_int_value(mrb, data->x);
}

static MRB_FUNC(IVecFirstSet)
{
        mrb_int arg;
        mrb_get_args(mrb, "i", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::ivec2);
        data->x = arg;

        return MRB_NIL;
}

static MRB_FUNC(IVecSecond)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::ivec2);
        return mrb_int_value(mrb, data->y);
}

static MRB_FUNC(IVecSecondSet)
{
        mrb_int arg;
        mrb_get_args(mrb, "i", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::ivec2);
        data->y = arg;

        return MRB_NIL;
}

static MRB_FUNC(IVecThird)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::ivec3);
        return mrb_int_value(mrb, data->z);
}

static MRB_FUNC(IVecThirdSet)
{
        mrb_int arg;
        mrb_get_args(mrb, "i", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::ivec3);
        data->z = arg;
        return MRB_NIL;
}

static MRB_FUNC(IVecFourth)
{
        auto data = MRB_ISTRUCT_VAL(self, glm::ivec4);
        return mrb_int_value(mrb, data->w);
}

static MRB_FUNC(IVecFourthSet)
{
        mrb_int arg;
        mrb_get_args(mrb, "i", &arg);

        auto data = MRB_ISTRUCT_VAL(self, glm::ivec4);
        data->w = arg;
        return MRB_NIL;
}

static MRB_FUNC(IVec2FromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        if (len != 2)
                BOREK_ENGINE_ERROR("Creating Vec2 from incompatible size: {}", len);

        mrb_value instance = mrb_class_new_instance(
                mrb, len, values, RBDatatypes::ivec2_class);

        return instance;
}

static MRB_FUNC(IVec2Initialize)
{
        mrb_int x = 0, y = 0;
        mrb_get_args(mrb, "|ii", &x, &y);

        auto* data = MRB_ISTRUCT_VAL(self, glm::ivec2);
        data->x = x;
        data->y = y;

        return self;
}

static MRB_FUNC(IVec2ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, glm::ivec2);
        std::string str = std::format("IVec2[{}, {}]", data->x, data->y);
        return mrb_str_new_cstr(mrb, str.c_str());
}

static MRB_FUNC(IVec3FromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        if (len != 3)
                BOREK_ENGINE_ERROR("Creating Vec3 from incompatible size: {}", len);

        mrb_value instance = mrb_class_new_instance(
                mrb, len, values, RBDatatypes::ivec3_class);

        return instance;
}

static MRB_FUNC(IVec3Initialize)
{
        mrb_int x = 0, y = 0, z = 0;
        mrb_get_args(mrb, "|iii", &x, &y, &z);

        auto* data = MRB_ISTRUCT_VAL(self, glm::ivec3);
        data->x = x;
        data->y = y;
        data->z = y;

        return self;
}

static MRB_FUNC(IVec3ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, glm::ivec3);
        std::string str = std::format("IVec3[{}, {}, {}]", data->x, data->y,
                                      data->z);
        return mrb_str_new_cstr(mrb, str.c_str());
}

static MRB_FUNC(IVec4FromItems)
{
        mrb_value* values;
        mrb_int len;
        mrb_get_args(mrb, "*!", &values, &len);

        if (len != 4)
                BOREK_ENGINE_ERROR("Creating Vec4 from incompatible size: {}", len);

        mrb_value instance = mrb_class_new_instance(
                mrb, len, values, RBDatatypes::ivec4_class);

        return instance;
}

static MRB_FUNC(IVec4Initialize)
{
        mrb_int x = 0, y = 0, z = 0, w = 0;
        mrb_get_args(mrb, "|iiii", &x, &y, &z, &w);

        auto* data = MRB_ISTRUCT_VAL(self, glm::ivec4);
        data->x = x;
        data->y = y;
        data->z = y;
        data->w = w;

        return self;
}

static MRB_FUNC(IVec4ToS)
{
        auto* data = MRB_ISTRUCT_VAL(self, glm::ivec4);
        std::string str = std::format("IVec4[{}, {}, {}, {}]", data->x, data->y,
                                      data->z, data->w);
        return mrb_str_new_cstr(mrb, str.c_str());
}

IVEC_ARITHMETIC_OPERATIONS(IVec2, ivec2)
IVEC_ARITHMETIC_OPERATIONS(IVec3, ivec3)
IVEC_ARITHMETIC_OPERATIONS(IVec4, ivec4)


void RBDatatypes::Init(RubyEngine& engine)
{
        VM& vm = engine.GetRubyVM();

        vec_class = vm.define_class("Vec")
                      .define_class_method("[]", VecFromItems, FuncArgs().Rest());

        vec2_class = vm.define_class("Vec2", MRB_TT_ISTRUCT)
                .define_method("initialize", Vec2Initialize, FuncArgs().Optional(2))
                .define_class_method("[]", Vec2FromItems, FuncArgs().Rest())
                .define_method("x", VecFirst)
                .define_method("x=", VecFirstSet, FuncArgs().Required(1))
                .define_method("y", VecSecond)
                .define_method("y=", VecSecondSet, FuncArgs().Required(1))
                .define_method("+", Vec2Add, FuncArgs().Required(1))
                .define_method("-", Vec2Sub, FuncArgs().Required(1))
                .define_method("*", Vec2Mul, FuncArgs().Required(1))
                .define_method("/", Vec2Div, FuncArgs().Required(1))
                .define_method("to_s", Vec2ToS);

        vec3_class = vm.define_class("Vec3", MRB_TT_ISTRUCT)
                .define_method("initialize", Vec3Initialize, FuncArgs().Optional(3))
                .define_class_method("[]", Vec3FromItems, FuncArgs().Rest())
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
                .define_method("+", Vec3Add, FuncArgs().Required(1))
                .define_method("-", Vec3Sub, FuncArgs().Required(1))
                .define_method("*", Vec3Mul, FuncArgs().Required(1))
                .define_method("/", Vec3Div, FuncArgs().Required(1))
                .define_method("to_s", Vec3ToS);

        vec4_class = vm.define_class("Vec4", MRB_TT_ISTRUCT)
                .define_method("initialize", Vec4Initialize, FuncArgs().Optional(4))
                .define_class_method("[]", Vec4FromItems, FuncArgs().Rest())
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
                .define_method("+", Vec4Add, FuncArgs().Required(1))
                .define_method("-", Vec4Sub, FuncArgs().Required(1))
                .define_method("*", Vec4Mul, FuncArgs().Required(1))
                .define_method("/", Vec4Div, FuncArgs().Required(1))
                .define_method("to_s", Vec4ToS);

        ivec_class = vm.define_class("IVec")
                       .define_class_method("[]", IVecFromItems, FuncArgs().Rest());

        ivec2_class = vm.define_class("IVec2", MRB_TT_ISTRUCT)
                .define_method("initialize", IVec2Initialize, FuncArgs().Optional(2))
                .define_class_method("[]", IVec2FromItems, FuncArgs().Rest())
                .define_method("x", IVecFirst)
                .define_method("x=", IVecFirstSet, FuncArgs().Required(1))
                .define_method("y", IVecSecond)
                .define_method("y=", IVecSecondSet, FuncArgs().Required(1))
                .define_method("+", IVec2Add, FuncArgs().Required(1))
                .define_method("-", IVec2Sub, FuncArgs().Required(1))
                .define_method("*", IVec2Mul, FuncArgs().Required(1))
                .define_method("/", IVec2Div, FuncArgs().Required(1))
                .define_method("to_s", IVec2ToS);

        ivec3_class = vm.define_class("IVec3", MRB_TT_ISTRUCT)
                .define_method("initialize", IVec3Initialize, FuncArgs().Optional(3))
                .define_class_method("[]", IVec3FromItems, FuncArgs().Rest())
                .define_method("x", IVecFirst)
                .define_method("x=", IVecFirstSet, FuncArgs().Required(1))
                .define_method("y", IVecSecond)
                .define_method("y=", IVecSecondSet, FuncArgs().Required(1))
                .define_method("z", IVecThird)
                .define_method("z=", IVecThirdSet, FuncArgs().Required(1))
                .define_method("r", IVecFirst)
                .define_method("r=", IVecFirstSet, FuncArgs().Required(1))
                .define_method("g", IVecSecond)
                .define_method("g=", IVecSecondSet, FuncArgs().Required(1))
                .define_method("b", IVecThird)
                .define_method("b=", IVecThirdSet, FuncArgs().Required(1))
                .define_method("+", IVec3Add, FuncArgs().Required(1))
                .define_method("-", IVec3Sub, FuncArgs().Required(1))
                .define_method("*", IVec3Mul, FuncArgs().Required(1))
                .define_method("/", IVec3Div, FuncArgs().Required(1))
                .define_method("to_s", IVec3ToS);

        ivec4_class = vm.define_class("IVec4", MRB_TT_ISTRUCT)
                .define_method("initialize", IVec4Initialize, FuncArgs().Optional(4))
                .define_class_method("[]", IVec4FromItems, FuncArgs().Rest())
                .define_method("x", IVecFirst)
                .define_method("x=", IVecFirstSet, FuncArgs().Required(1))
                .define_method("y", IVecSecond)
                .define_method("y=", IVecSecondSet, FuncArgs().Required(1))
                .define_method("z", IVecThird)
                .define_method("z=", IVecThirdSet, FuncArgs().Required(1))
                .define_method("w", IVecFourth)
                .define_method("w=", IVecFourthSet, FuncArgs().Required(1))
                .define_method("r", IVecFirst)
                .define_method("r=", IVecFirstSet, FuncArgs().Required(1))
                .define_method("g", IVecSecond)
                .define_method("g=", IVecSecondSet, FuncArgs().Required(1))
                .define_method("b", IVecThird)
                .define_method("b=", IVecThirdSet, FuncArgs().Required(1))
                .define_method("a", IVecFourth)
                .define_method("a=", IVecFourthSet, FuncArgs().Required(1))
                .define_method("+", IVec4Add, FuncArgs().Required(1))
                .define_method("-", IVec4Sub, FuncArgs().Required(1))
                .define_method("*", IVec4Mul, FuncArgs().Required(1))
                .define_method("/", IVec4Div, FuncArgs().Required(1))
                .define_method("to_s", IVec4ToS);
}

Class RBDatatypes::vec_class;
Class RBDatatypes::vec2_class;
Class RBDatatypes::vec3_class;
Class RBDatatypes::vec4_class;
Class RBDatatypes::ivec_class;
Class RBDatatypes::ivec2_class;
Class RBDatatypes::ivec3_class;
Class RBDatatypes::ivec4_class;

}  // namespace RBModules
}  // namespace Borek
