// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include <functional>
#include <iostream>
#include <ostream>
#include <print>
#include <sstream>

namespace Borek {

struct Node {
        Node(Entity e) : entity(e) {}

        Entity entity;
        Node* parent = nullptr;
        Node* prev = nullptr;
        Node* next = nullptr;
        Node* first_child = nullptr;
        Node* last_child = nullptr;

        void RemoveFromList()
        {
                if (this == parent->first_child) {
                        if (this == parent->last_child) {
                                parent->first_child = nullptr;
                                parent->last_child = nullptr;
                        } else {
                                parent->first_child = next;
                                next->prev = nullptr;
                        }
                } else if (this == parent->last_child) {
                        parent->last_child = prev;
                        prev->next = nullptr;
                } else {
                        prev->next = next;
                        next->prev = prev;
                }
        }

        void ChildAppend(Node* child)
        {
                child->prev = nullptr;
                child->next = nullptr;

                if (last_child == nullptr) {
                        first_child = child;
                        last_child = child;
                } else {
                        last_child->next = child;
                        child->prev = last_child;
                        last_child = child;
                }

                child->parent = this;
        }

        void ChildPrepend(Node* child)
        {
                child->prev = nullptr;
                child->next = nullptr;

                if (first_child == nullptr) {
                        first_child = child;
                        last_child = child;
                } else {
                        first_child->prev = child;
                        child->next = first_child;
                        first_child = child;
                }

                child->parent = this;
        }

        void Move(Node* dest)
        {
                if (dest == this)
                        return;

                if (parent) RemoveFromList();

                parent = dest->parent;
                next = dest;
                prev = dest->prev;

                if (dest == parent->first_child) {
                        parent->first_child = this;
                } else {
                        prev->next = this;
                }
                next->prev = this;
        }

        void MoveBack(Node* dest)
        {
                if (dest == this)
                        return;

                if (parent) RemoveFromList();

                parent = dest->parent;
                prev = dest;
                next = dest->next;

                if (dest == parent->last_child) {
                        parent->last_child = this;
                } else {
                        next->prev = this;
                }
                prev->next = this;
        }

        void Reparent(Node* p)
        {
                if (parent == nullptr) {
                        if (p) p->ChildPrepend(this);
                        return;
                }

                RemoveFromList();

                if (p) p->ChildPrepend(this);
        }

        void ReparentBack(Node* p)
        {
                if (parent == nullptr) {
                        if (p) p->ChildAppend(this);
                        return;
                }

                RemoveFromList();

                if (p) p->ChildAppend(this);
        }

        void EachChild(std::function<void(Node*)> visitor)
        {
                Node* child = this->first_child;
                while (child) {
                        Node* next = child->next;
                        visitor(child);
                        child = next;
                }
        }

        void EachChildReverse(std::function<void(Node*)> visitor)
        {
                Node* child = this->last_child;
                while (child) {
                        Node* next = child->prev;
                        visitor(child);
                        child = next;
                }
        }

        void Delete()
        {
                entity.Delete();

                EachChild([](Node* child){
                        child->Delete();
                });

                delete this;
        }

        const std::string ToString()
        {
                std::stringstream res;
                res << "(ID: " << entity.m_Id;
                if (first_child) {
                        res << " CHILDREN: [";
                        EachChild([&res](Node* child){
                                res << child->ToString();
                        });
                        res << "]";
                }
                res << ")";
                return res.str();
        }
};

}  // namespace Borek
