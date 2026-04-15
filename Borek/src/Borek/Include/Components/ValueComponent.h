// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Colors.h"
#include <cstdint>

#include "Include/Base/Symbol.h"
#include "Include/Engine/Alloc.h"
#include "Include/Base/MsgConnection.h"
#include "Include/Engine/Utils/SmallVecSet.h"

namespace Borek {

enum ValueType_ {
        ValueType_Number,
        ValueType_Int,
        ValueType_String,
        ValueType_Bool,
        ValueType_Color,
        ValueType_Node,
        ValueType_Asset,
};

struct Value {
        Symbol name;
        ValueType_ type;
        union {
                Symbol str;
                bool boolean;
                int64_t integer;
                double number;
                Color color;
                struct { Symbol path; uint32_t entity_id; } node;
                struct { Symbol path; uint32_t asset_id; } asset;
        };

        Value();
        Value(const Symbol& name, ValueType_ type);
        Value(const Value& other);
        Value(Value&& other);
        ~Value();

        Value&
        operator =(const Value& other);

        Value&
        operator =(Value&& other);

        bool
        operator ==(const Value& other) const;
};

struct ValueComponent : public SmallVecSet<Value, 3> {
        AUniq<MsgConnection> on_change;

        ValueComponent();
        ValueComponent(const ValueComponent& other);
        ValueComponent(ValueComponent&& other);

        ValueComponent&
        operator =(const ValueComponent& other);

        ValueComponent&
        operator =(ValueComponent&& other);

        void
        Add(const Symbol& name, ValueType_ type);

        void
        Add(const Symbol& name, double val);

        void
        Add(const Symbol& name, int64_t val);

        void
        Add(const Symbol& name, bool val);

        void
        Add(const Symbol& name, Symbol val);

        Value*
        Get(const Symbol& name);

        void
        Remove(const Symbol& name);
};

}  // namespace Borek
