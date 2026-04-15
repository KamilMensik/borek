// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>

#include "Include/Engine/FZX/LTDGrid/SmallList.h"
#include "Include/Engine/SceneTree.h"

namespace Borek {

void
SceneTree::AddEntity(Entity e, const Symbol& name, NodeType type, Entity parent)
{
        EntityData& ed = m_EntityData[e];
        ed.name = name;

        ReparentEntity(e, parent, ed);
}

void
SceneTree::RemoveEntity(Entity e)
{
        EntityData& ed = m_EntityData[e];

        auto it = std::find(m_SceneGraph[ed.parent].begin(),
                            m_SceneGraph[ed.parent].end(), e);

        m_SceneGraph[ed.parent].erase(it);
        m_SceneGraphByName.erase({ ed.name, ed.parent });
        m_EntityData.erase(e);
}

void
SceneTree::TraverseScene(iteration_func iter_func) const
{
        TraverseScene(m_RootEntity, iter_func);
}

void
SceneTree::TraverseSceneReverse(iteration_func iter_func) const
{
        TraverseSceneReverse(m_RootEntity, iter_func);
}

void
SceneTree::TraverseScene(iteration_func before, iteration_func after) const
{
        TraverseScene(m_RootEntity, before, after);
}

void
SceneTree::TraverseSceneReverse(iteration_func before, iteration_func after) const
{
        TraverseSceneReverse(m_RootEntity, before, after);
}

void
SceneTree::TraverseScene(Entity e, iteration_func iter_func) const
{
        iter_func(e);

        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : std::ranges::views::reverse(children->second)) {
                        TraverseScene(entity_id, iter_func);
                }
        }
}

void
SceneTree::TraverseSceneReverse(Entity e, iteration_func iter_func) const
{
        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : children->second) {
                        TraverseScene(entity_id, iter_func);
                }
        }

        iter_func(e);
}

void
SceneTree::TraverseScene(Entity e, iteration_func before,
                               iteration_func after) const
{
        before(e);

        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : std::ranges::views::reverse(children->second)) {
                        TraverseScene(entity_id, before, after);
                }
        }

        after(e);
}

void
SceneTree::TraverseSceneReverse(Entity e, iteration_func before,
                                      iteration_func after) const
{
        before(e);

        if (auto children = m_SceneGraph.find(e.GetId()); children != m_SceneGraph.end()) {
                for (uint32_t entity_id : std::ranges::views::reverse(children->second)) {
                        TraverseScene(entity_id, before, after);
                }
        }

        after(e);
}

Entity
SceneTree::EntityFindChild(Symbol name, Entity parent) const
{
        if (parent.IsNil())
                parent = m_RootEntity;

        auto it = m_SceneGraphByName.find({ name, parent });
        if (it == m_SceneGraphByName.end())
                return Entity();

        return Entity(it->second);
}

void
SceneTree::EntityAppendChild(Entity entity, Entity dest)
{
        EntityData& ed = m_EntityData[entity];

        ReparentEntity(entity, dest, ed);
        m_SceneGraph[dest].emplace_back(entity);
}

void
SceneTree::EntityPrependChild(Entity entity, Entity dest)
{
        EntityData& ed = m_EntityData[entity];
        std::vector<Entity>& dest_children = m_SceneGraph[dest];

        ReparentEntity(entity, dest, ed);
        dest_children.insert(dest_children.begin(), entity);
}

void
SceneTree::EntityAppend(Entity entity, Entity dest)
{
        EntityData& ed = m_EntityData[entity];
        const Entity dest_parent = m_EntityData[dest].parent;
        std::vector<Entity>& dest_children = m_SceneGraph[dest_parent];

        ReparentEntity(entity, dest_parent, ed);
        auto it = std::find(dest_children.begin(), dest_children.end(), dest);
        m_SceneGraph[dest_parent].insert(it + 1, entity);
}

void
SceneTree::EntityPrepend(Entity entity, Entity dest)
{
        EntityData& ed = m_EntityData[entity];
        const Entity dest_parent = m_EntityData[dest].parent;
        std::vector<Entity>& dest_children = m_SceneGraph[dest_parent];

        ReparentEntity(entity, dest_parent, ed);
        auto it = std::find(dest_children.begin(), dest_children.end(), dest);
        m_SceneGraph[dest_parent].insert(it, entity);
}

Entity
SceneTree::GetRootEntity() const
{
        return m_RootEntity;
}

void
SceneTree::SetRootEntity(Entity root)
{
        m_RootEntity = root;
        m_EntityData[root].parent = Entity();
        m_EntityData[root].parent_count = 0;
}

Entity
SceneTree::GetEntityParent(Entity e) const
{
        auto it = m_EntityData.find(e);
        if (it == m_EntityData.end())
                return Entity();

        return it->second.parent;
}

uint32_t
SceneTree::GetEntityParentCount(Entity e) const
{
        auto it = m_EntityData.find(e);
        if (it == m_EntityData.end())
                return UINT32_MAX;

        return it->second.parent_count;
}

Entity
SceneTree::GetEntityPrevSibling(Entity e) const
{
        auto& ed = m_EntityData.find(e)->second;
        Entity parent = ed.parent;
        if (parent.IsNil())
                return Entity();

        auto& siblings = m_SceneGraph.find(parent)->second;

        auto it = std::ranges::find(siblings, e);
        if (it == siblings.begin())
                return Entity();

        return *--it;
}

Entity
SceneTree::GetEntityNextSibling(Entity e) const
{
        auto& ed = m_EntityData.find(e)->second;
        Entity parent = ed.parent;
        if (parent.IsNil())
                return Entity();

        auto& siblings = m_SceneGraph.find(parent)->second;

        auto it = ++std::ranges::find(siblings, e);
        if (it == siblings.end())
                return Entity();

        return *it;
}

const Symbol&
SceneTree::GetEntityName(Entity e) const
{
        static const Symbol nil;

        auto it = m_EntityData.find(e);
        if (it == m_EntityData.end())
                return nil;

        return it->second.name;
}

void
SceneTree::SetEntityName(Entity e, const Symbol& sym, bool force)
{
        auto& data = m_EntityData[e];
        if (!force & (data.name == sym)) return;

        auto it = m_SceneGraphByName.find({sym, data.parent});
        if (it == m_SceneGraphByName.end()) {
                m_EntityData[e].name = sym;
                m_SceneGraphByName[{ sym, data.parent }] = e;
                return;
        }

        std::string_view name_full(sym.Str());
        auto name_end = name_full.find_last_of(' ');

        for (int i = 1;;i++) {
                std::string new_name(name_full.substr(0, name_end));
                new_name += ' ';
                new_name.append(std::to_string(i));

                Symbol new_name_s(new_name);

                auto it = m_SceneGraphByName.find({new_name_s, data.parent});
                if (it == m_SceneGraphByName.end()) {
                        m_EntityData[e].name = new_name_s;
                        m_SceneGraphByName[{ new_name_s, data.parent }] = e;
                        return;
                }
        }
}

NodeType
SceneTree::GetEntityNodeType(Entity e) const
{
        auto it = m_EntityData.find(e);
        if (it == m_EntityData.end())
                return NodeType::Node;

        return it->second.node_type;
}

void
SceneTree::SetEntityNodeType(Entity e, NodeType type)
{
        m_EntityData[e].node_type = type;
}

uint64_t
SceneTree::GetEntityRubyNode(Entity e) const
{
        auto it = m_EntityData.find(e);
        if (it == m_EntityData.end())
                return UINT64_MAX;

        return it->second.ruby_node;
}

void
SceneTree::SetEntityRubyNode(Entity e, uint64_t ruby_node)
{
        m_EntityData[e].ruby_node = ruby_node;
}


uint64_t
SceneTree::GetEntityCount() const
{
        return m_EntityData.size();
}

const std::vector<Entity>*
SceneTree::GetEntityChildren(Entity e) const
{
        auto it = m_SceneGraph.find(e);
        if (it == m_SceneGraph.end())
                return nullptr;

        return &it->second;
}

bool
SceneTree::HasEntityChildren(Entity e) const
{
        auto children = GetEntityChildren(e);
        return children != nullptr && !children->empty();
}

bool
SceneTree::IsEntityParentOf(Entity a, Entity b) const
{
        const EntityData* ed = &m_EntityData.find(b)->second;

        while (!ed->parent.IsNil()) {
                if (ed->parent == a) return true;

                ed = &m_EntityData.find(ed->parent)->second;
        }

        return false;
}

std::string
SceneTree::PathTo(Entity a, Entity b) const
{
        const EntityData* a_ed = &m_EntityData.find(a)->second;
        const EntityData* b_ed = &m_EntityData.find(b)->second;

        uint32_t a_depth = a_ed->parent_count;
        uint32_t b_depth = b_ed->parent_count;

        std::stringstream res;
        res << ".";

        std::vector<Symbol> path;
        path.reserve(b_depth);

        while (true) {
                if (a_ed == b_ed) break;
                if (a_depth + b_depth == 0) return "";

                if (a_depth < b_depth) {
                        b_depth--;
                        path.emplace_back(b_ed->name);
                        b_ed = &m_EntityData.find(b_ed->parent)->second;
                } else if (a_depth > b_depth) {
                        a_depth--;
                        res << "/..";
                        a_ed = &m_EntityData.find(a_ed->parent)->second;
                } else {
                        b_depth--;
                        path.emplace_back(b_ed->name);
                        b_ed = &m_EntityData.find(b_ed->parent)->second;
                        a_depth--;
                        res << "/..";
                        a_ed = &m_EntityData.find(a_ed->parent)->second;
                }
        }

        for (const Symbol& name : std::ranges::views::reverse(path)) {
                res << "/" << name.Str();
        }

        return res.str();
}

Entity
SceneTree::FindEntityByPath(Entity from, std::string_view path) const
{
        Entity res;

        for (auto it : path | std::ranges::views::split('/')) {
                const std::string_view val(it);
                if (val == ".")
                        res = from;
                else if (val == "..")
                        res = m_EntityData.find(from)->second.parent;
                else
                        res = EntityFindChild(val, res);
        }

        return res;
}

static void
entity_to_s(std::stringstream& out, Entity e, unsigned depth) {
        for (unsigned i = 0; i < depth; i++) {
                out << "  ";
        }

        out << "Entity (" << e.GetId() << ")" << "\n";

        const std::vector<Entity>* children = e.GetChildren();
        if (children == nullptr)
                return;

        for (Entity e : *children) {
                entity_to_s(out, e, depth + 1);
        }
}

std::string
SceneTree::ToS() const
{
        std::stringstream ss;
        entity_to_s(ss, m_RootEntity, 0);
        return ss.str();
}

void
SceneTree::Destroy(const std::function<void(Entity)>& fn) &&
{
        Destroy(m_RootEntity, fn);
        m_SceneGraph.clear();
        m_SceneGraphByName.clear();
        m_EntityData.clear();
}

void
SceneTree::DestroyEntity(Entity e, Entity parent)
{
        bool was_parent_nil = parent.IsNil();
        if (was_parent_nil) parent = GetEntityParent(e);

        auto children = GetEntityChildren(e);
        if (children != nullptr) {
                for (Entity child : *children) {
                        DestroyEntity(child, parent);
                }

                m_SceneGraph.erase(e);
        }

        m_SceneGraphByName.erase({ GetEntityName(e), parent });
        m_EntityData.erase(e);

        if (was_parent_nil) {
                auto& p_children = m_SceneGraph[parent];
                p_children.erase(std::ranges::find(p_children, e));
        }
}

void
SceneTree::Destroy(Entity e, const std::function<void(Entity)>& fn)
{
        auto children = GetEntityChildren(e);
        if (children != nullptr) {
                for (Entity e : *children) {
                        Destroy(e, fn);
                }
        }

        fn(e);
}

void
SceneTree::ReparentEntity(Entity e, Entity parent, EntityData& ed)
{
        if (!ed.parent.IsNil()) {
                auto it = std::find(m_SceneGraph[ed.parent].begin(),
                                    m_SceneGraph[ed.parent].end(), e);

                m_SceneGraph[ed.parent].erase(it);
        }

        m_SceneGraphByName.erase({ ed.name, ed.parent });
        ed.parent = parent;
        ed.parent_count = m_EntityData[parent].parent_count + 1;
        SetEntityName(e, ed.name, true);

        FZX::SmallList<uint32_t> entity_stack;
        entity_stack.push_back(e);

        while (entity_stack.size() > 0) {
                Entity e = entity_stack.pop_back();
                auto it = m_SceneGraph.find(e);
                if (it == m_SceneGraph.end()) continue;

                uint32_t parent_count = m_EntityData[e].parent_count;
                for (Entity child : it->second) {
                        m_EntityData[child].parent_count = parent_count + 1;
                        entity_stack.push_back(child);
                }
        }

}

}  // namespace Borek
