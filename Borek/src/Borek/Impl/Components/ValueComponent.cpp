// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Colors.h"
#include "Include/Base/MsgConnection.h"
#include "Include/Engine/Alloc.h"
#include "Include/Engine/Assets/Asset.h"
#include <memory>

#include "Include/Components/ValueComponent.h"

namespace Borek {

Value::Value()
{
}

Value::Value(const Symbol& name, ValueType_ type)
{
        this->name = name;
        this->type = type;

        switch (type) {
        case ValueType_Number:
                number = 0.0;
                break;
        case ValueType_String:
                std::construct_at(&str, "");
                break;
        case ValueType_Int:
                integer = 0;
                break;
        case ValueType_Bool:
                boolean = false;
                break;
        case ValueType_Color:
                color = Colors::WHITE;
                break;
        case ValueType_Node:
                std::construct_at(&node.path);
                node.entity_id = UINT32_MAX;
                break;
        case ValueType_Asset:
                std::construct_at(&node.path);
                node.entity_id = UINT32_MAX;
                break;
        }
}

Value::Value(const Value& other)
{
        name = other.name;
        type = other.type;

        switch (other.type) {
        case ValueType_String:
                std::construct_at(&str, other.str);
                break;
        case ValueType_Bool:
                boolean = other.boolean;
                break;
        case ValueType_Int:
                integer = other.integer;
                break;
        case ValueType_Number:
                number = other.number;
                break;
        case ValueType_Color:
                color = other.color;
                break;
        case ValueType_Node:
                std::construct_at(&node.path, other.node.path);
                node.entity_id = other.node.entity_id;
                break;
        case Borek::ValueType_Asset:
                std::construct_at(&asset.path, other.asset.path);
                asset.asset_id = other.asset.asset_id;
                if (asset.asset_id != UINT32_MAX)
                        AssetManager::Increment(asset.asset_id);
                break;
        }
}

Value::Value(Value&& other)
{
        name = std::move(other.name);
        type = other.type;

        switch (other.type) {
        case ValueType_String:
                std::construct_at(&str, std::move(other.str));
                break;
        case ValueType_Bool:
                boolean = other.boolean;
                break;
        case ValueType_Int:
                integer = other.integer;
                break;
        case ValueType_Number:
                number = other.number;
                break;
        case ValueType_Color:
                color = other.color;
                break;
        case ValueType_Node:
                std::construct_at(&node.path, std::move(other.node.path));
                node.entity_id = other.node.entity_id;
                break;
        case Borek::ValueType_Asset:
                std::construct_at(&asset.path, std::move(other.asset.path));
                asset.asset_id = other.asset.asset_id;
                break;
        }
}

Value::~Value()
{
        switch (type) {
        case ValueType_String:
                str.~Symbol();
                break;
        case ValueType_Node:
                node.path.~Symbol();
                break;
        case Borek::ValueType_Asset:
                asset.path.~Symbol();
                if (asset.asset_id != UINT32_MAX)
                        AssetManager::Decrement(asset.asset_id);
                break;
        default:
                break;
        }
}

Value&
Value::operator =(const Value& other)
{
        this->~Value();
        std::construct_at(this, other);

        return *this;
}

Value&
Value::operator =(Value&& other)
{
        this->~Value();
        std::construct_at(this, other);

        return *this;
}

bool
Value::operator ==(const Value& other) const
{
        return name == other.name;
}

ValueComponent::ValueComponent()
{
        on_change = NewAUniq<MsgConnection>();
}

ValueComponent::ValueComponent(const ValueComponent& other) : SmallVecSet<Value, 3>(other)
{
        on_change = NewAUniq<MsgConnection>(*other.on_change);
}

ValueComponent::ValueComponent(ValueComponent&& other) : SmallVecSet<Value, 3>(other)
{
        on_change = std::move(other.on_change);
}

ValueComponent&
ValueComponent::operator =(const ValueComponent& other)
{
        on_change = NewAUniq<MsgConnection>(*other.on_change);
        SmallVecSet<Value, 3>::operator=(other);

        return *this;
}

ValueComponent&
ValueComponent::operator =(ValueComponent&& other)
{
        on_change = std::move(other.on_change);
        SmallVecSet<Value, 3>::operator=(other);

        return *this;
}

void
ValueComponent::Add(const Symbol& name, ValueType_ type)
{
        Value new_val(name, type);

        insert(std::move(new_val));
}

void
ValueComponent::Add(const Symbol& name, double val)
{
        Value new_val(name, ValueType_Number);
        new_val.number = val;

        insert(std::move(new_val));
}

void
ValueComponent::Add(const Symbol& name, int64_t val)
{
        Value new_val(name, ValueType_Int);
        new_val.integer = val;

        insert(std::move(new_val));
}

void
ValueComponent::Add(const Symbol& name, bool val)
{
        Value new_val(name, ValueType_Bool);
        new_val.boolean = val;

        insert(std::move(new_val));
}

void
ValueComponent::Add(const Symbol& name, Symbol val)
{
        Value new_val(name, ValueType_String);
        new_val.str = val;

        insert(std::move(new_val));
}

Value*
ValueComponent::Get(const Symbol& name)
{
        Value test(name, ValueType_Number);
        return get(test);
}

void
ValueComponent::Remove(const Symbol& name)
{
        Value test(name, ValueType_Number);
        erase(test);
}

}  // namespace Borek
